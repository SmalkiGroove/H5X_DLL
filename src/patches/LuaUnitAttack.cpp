#include "pch.h"

// LUA: UnitAttack(attacker, defender) combat script callback
// Called with the script names of both units (the names UnitMove / UnitDeath receive) after each attack:
// - creature and war machine attacks: func_CreatureAttackEffects (0x00A65260, called by func_CombatAttackWithRedirects
//   with the final target, after the redirections), one call per attack action on the main target,
// - hero attacks: FUN_008CEAC0 (hero default attack, called by func_ComputeHeroAttackEffects).
// Attacks of a silent / estimate combat (CCombat vtable +0x108, no combat events either) are ignored.
// The attack code is game logic: running Lua there would interleave scripts with the damage resolution. The pair is
// queued instead, and the queue is run by the combat display loop (FUN_0054AD10, 0x0054B2D4), just before the
// loop that runs UnitDeath: like UnitDeath, the call does not wait for the script, the attack is already resolved
// (damage dealt, deaths applied), and UnitAttack runs before the UnitDeath calls of the same events.
// The call is "if UnitAttack then UnitAttack("a","d") end"; the game still prints a "Value was NIL" warning for each
// attack when the global is missing, so combat-startup.lua defines an empty UnitAttack.

int __fastcall unit_attack_record(ICombatUnit* attacker, ICombatUnit* defender);
void __fastcall unit_attack_flush(char* manager);
void UnitAttackCreatureFork();
void UnitAttackHeroFork();
void UnitAttackFlushFork();

int UnitAttackCreature_fork = 0x00A65260;    // func_CreatureAttackEffects entry: sub esp,50h / push ebx / push ebp / push esi
int UnitAttackCreature_return = 0x00A65266;
int UnitAttackHero_fork = 0x008CEAC0;        // hero default attack entry: sub esp,0Ch / push ebx / push ebp
int UnitAttackHero_return = 0x008CEAC5;
int UnitAttackFlush_fork = 0x0054B2D4;       // FUN_0054AD10, after the display flag 0x40 test: mov eax,[esp+48h] / sub eax,[esp+44h]
int UnitAttackFlush_return = 0x0054B2DC;

void LuaUnitAttack_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, UnitAttackCreature_fork, 6, UnitAttackCreatureFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, UnitAttackHero_fork, 5, UnitAttackHeroFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, UnitAttackFlush_fork, 8, UnitAttackFlushFork, 0, 0, 0 });
}

// Script manager (func_CombatDeath's this, FUN_0054AD10's this - 0x564)
static const int manager_combat = 0x2A0;     // CCombat: vtable +0x120 = script names
static const int manager_runner = 0x1B4;     // script runner: vtable +0x00 = thiscall(const char* code), no wait
static const int manager_scripted = 0x4F0;   // byte: the combat has a script
static const size_t queue_limit = 64;

struct UnitAttackCall {
	std::string attacker;
	std::string defender;
};

static std::vector<UnitAttackCall> unit_attack_queue;
static int* unit_attack_combat = nullptr;    // combat of the last flush

// Script name of a unit: names object (CCombat vtable +0x120), vtable +0x08 = thiscall(CombatUnit) -> {begin, end}
static std::string unit_script_name(int* combat, ICombatUnit* unit) {
	typedef int*(__thiscall* NamesFn)(int*);
	typedef char**(__thiscall* NameFn)(int*, ICombatUnit*);
	int* names = ((NamesFn)(*(int***)combat)[0x120 / 4])(combat);
	if (!names) return std::string();
	char** range = ((NameFn)(*(int***)names)[0x08 / 4])(names, unit);
	if (!range || !range[0] || range[1] <= range[0]) return std::string();
	return std::string(range[0], range[1]);
}

// Lua string literal: the names are map script names, quotes and backslashes are escaped anyway
static void append_lua_string(std::string& out, const std::string& value) {
	out += '"';
	for (char c : value) {
		if (c == '"' || c == '\\') out += '\\';
		out += c;
	}
	out += '"';
}

int __fastcall unit_attack_record(ICombatUnit* attacker, ICombatUnit* defender) {
	if (!attacker || !defender || attacker == defender) return 0;
	int* combat = attacker->combat();
	if (!combat) return 0;
	typedef char(__thiscall* CombatFlagFn)(int*);
	if (((CombatFlagFn)(*(int***)combat)[0x108 / 4])(combat)) return 0;
	if (unit_attack_queue.size() >= queue_limit) return 0;

	UnitAttackCall call = { unit_script_name(combat, attacker), unit_script_name(combat, defender) };
	if (call.attacker.empty() || call.defender.empty()) return 0;
	unit_attack_queue.push_back(call);
	return 0;
}

void __fastcall unit_attack_flush(char* manager) {
	// A new combat (other CCombat) drops what an ended combat left unflushed
	int* combat = *(int**)(manager + manager_combat);
	if (combat != unit_attack_combat) {
		unit_attack_combat = combat;
		unit_attack_queue.clear();
		return;
	}
	if (unit_attack_queue.empty()) return;

	std::vector<UnitAttackCall> calls;
	calls.swap(unit_attack_queue);
	if (!manager[manager_scripted]) return;

	int* runner = (int*)(manager + manager_runner);
	typedef void(__thiscall* RunFn)(int*, const char*);
	for (const UnitAttackCall& call : calls) {
		std::string code = "if UnitAttack then UnitAttack(";
		append_lua_string(code, call.attacker);
		code += ',';
		append_lua_string(code, call.defender);
		code += ") end";
		((RunFn)(*(int***)runner)[0])(runner, code.c_str());
	}
}

// Entry of func_CreatureAttackEffects: [esp+8] = attacker, [esp+0Ch] = target (CombatUnit, may be null)
__declspec(naked) void UnitAttackCreatureFork() {
	__asm
	{
		pushad
		mov ecx, dword ptr [esp + 0x28]
		mov edx, dword ptr [esp + 0x2C]
		call unit_attack_record
		popad
		sub esp, 0x50
		push ebx
		push ebp
		push esi
		jmp [UnitAttackCreature_return]
	}
}

// Entry of the hero default attack: ecx = combat hero object (vbptr at +4, CombatUnit = virtual base 2), eax = target
__declspec(naked) void UnitAttackHeroFork() {
	__asm
	{
		pushad
		mov edx, eax
		mov eax, dword ptr [ecx + 4]
		mov eax, dword ptr [eax + 8]
		lea ecx, [eax + ecx + 4]
		call unit_attack_record
		popad
		sub esp, 0xC
		push ebx
		push ebp
		jmp [UnitAttackHero_return]
	}
}

// esi = FUN_0054AD10's this; the script manager is at esi - 0x564 (lea ecx,[esi-564h] before func_CombatDeath)
__declspec(naked) void UnitAttackFlushFork() {
	__asm
	{
		pushad
		lea ecx, [esi - 0x564]
		call unit_attack_flush
		popad
		mov eax, dword ptr [esp + 0x48]
		sub eax, dword ptr [esp + 0x44]
		jmp [UnitAttackFlush_return]
	}
}
