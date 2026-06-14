#include "pch.h"
#include "types/Abilities.h"
#include "types/Functions.h"

// Magnetism (ABILITY_MAGNETIC_FIELD): when an adjacent ally is targeted by a
// ranged attack, a nearby stack with this ability may redirect the shot to itself.
//
// Hook in func_CombatAttackWithRedirects @ 0x00A65D50, immediately after the vanilla Taunt
// (0x70) loop and before the bodyguard (0x41) block.
//
// Skipped for Griffin Dive effect kinds 0x9B / 0x15B (untargetable dive phase), same as Taunt.

namespace {

using CollectAdjacentUnitsFn = void(__fastcall*)(int* refUnit, int* outVec);
using AbilityProcSetupFn = void(__fastcall*)(int, int);
using AbilityProcRollFn = char(__fastcall*)(int*, int*);
using EngineFreeFn = void(__stdcall*)(void*);

CollectAdjacentUnitsFn collect_adjacent_units = (CollectAdjacentUnitsFn)0x00B3D560;
AbilityProcSetupFn ability_proc_setup = (AbilityProcSetupFn)0x00A59990;
AbilityProcRollFn ability_proc_roll = (AbilityProcRollFn)0x00A5A170;
EngineFreeFn engine_free = (EngineFreeFn)0x00879240;
StdcallPtr_Uint engine_alloc = (StdcallPtr_Uint)0x00878FD0;

using CombatEventWrapFn = int*(__thiscall*)(
	int* eventMem, int attackerTile, int* attacker, int* magnetUnit,
	int abilityId, int, int, int, int);
CombatEventWrapFn wrap_combat_event = (CombatEventWrapFn)0x00C9AE60;

using PostSideCombatEventFn = void(__thiscall*)(int* side, int* event);
using SideEventFactoryFn = int*(__thiscall*)(int, int* attacker, int* unit, int abilityId, int, int, int, int);

static bool unit_has_ability(int* unit, int abilityId) {
	if (!unit)
		return false;
	auto* vt = *(void***)unit;
	auto hasAbility = (HasCreatureAbility)vt[0x28C / 4];
	return hasAbility(unit, abilityId) != 0;
}

static int* get_side_from_attacker(int* attacker) {
	if (!attacker)
		return nullptr;
	auto* vt = *(void***)attacker;
	auto getSide = (GetUnitRef)vt[0x08 / 4];
	return getSide(attacker);
}

static void collect_adjacent_allies(int* refUnit, int tileX, int tileY, int outVec[3]) {
	outVec[0] = 0;
	outVec[1] = 0;
	outVec[2] = 0;
	__asm {
		push 0
		push 1
		push 1
		push 1
		push tileY
		push tileX
		lea edx, outVec
		mov ecx, refUnit
		call collect_adjacent_units
	}
}

static int* create_magnetism_event(int* side, int attackerTile, int* attacker, int* magnetUnit) {
	if (!side)
		return nullptr;
	int** sideVt = *(int***)side;
	int* miThis = (int*)(*(int*)(sideVt[1] + 8) + 4 + (intptr_t)side);
	auto factory = (SideEventFactoryFn)(*(void***)miThis)[0];
	int* mem = (int*)engine_alloc(0x5C);
	if (!mem)
		return nullptr;
	int* partial = factory(attackerTile, attacker, magnetUnit, ABILITY_MAGNETIC_FIELD, 0, 0, 0, 1);
	return wrap_combat_event(
		mem, partial, attackerTile, attacker, magnetUnit,
		ABILITY_MAGNETIC_FIELD, 0, 0, 0, 1);
}

static void post_side_event(int* side, int* event) {
	if (!side || !event)
		return;
	auto post = (PostSideCombatEventFn)(*(void***)side)[0x100 / 4];
	post(side, event);
}

} // namespace

static int* __cdecl magnetism_try_redirect(
	int* attacker,
	int* target,
	int attackerTile,
	int damageCtx,
	int tileX,
	int tileY,
	char skipRedirect,
	int effectKind,
	int isRanged)
{
	if (!attacker || !target || skipRedirect != 0)
		return target;
	if (effectKind == 0x9B || effectKind == 0x15B)
		return target;
	if (!isRanged)
		return target;

	int vec[3] = {};
	collect_adjacent_allies(target, tileX, tileY, vec);

	int* begin = (int*)vec[0];
	int* end = (int*)vec[1];
	int* side = get_side_from_attacker(attacker);
	int* redirected = target;

	if (begin && end > begin) {
		int count = (int)(end - begin);
		for (int i = 0; i < count; ++i) {
			int* candidate = ((int**)begin)[i];
			if (!candidate || candidate == target)
				continue;
			if (!unit_has_ability(candidate, ABILITY_MAGNETIC_FIELD))
				continue;

			int* stats = get_defaultstats();
			ability_proc_setup(1, *(int*)((char*)stats + 0x52C));
			if (!ability_proc_roll((int*)0, (int*)damageCtx))
				continue;

			int* event = create_magnetism_event(side, attackerTile, attacker, candidate);
			post_side_event(side, event);
			redirected = candidate;
			break;
		}
	}

	if (begin)
		engine_free(begin);
	return redirected;
}

void MagnetismFork();

// Replaces: mov ebx, dword ptr [esp + 0x9C]  (7 bytes @ 0x00A65EAA)
int Magnetism_fork = 0x00A65EAA;
int Magnetism_continue = 0x00A65EB3;

void Magnetism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Magnetism_fork, 7, MagnetismFork, 0, 0, 0 });
}

__declspec(naked) void MagnetismFork() {
	__asm {
		push esi
		push edi

		// Stack layout matches func_CombatAttackWithRedirects after push esi/edi (+8).
		push dword ptr [esp + 0xC0] // param_10 — 1 on ranged resolve (FUN_00979930)
		push dword ptr [esp + 0xB4] // param_7  — effect kind
		push dword ptr [esp + 0xAC] // param_5  — skip-redirect flag
		push dword ptr [esp + 0xBC] // param_9  — target tile Y
		push dword ptr [esp + 0xB8] // param_8  — target tile X
		push dword ptr [esp + 0xA8] // param_4  — damage / roll context
		push dword ptr [esp + 0x1C] // local_84 — attacker tile index
		push dword ptr [esp + 0xA4] // param_3  — current target stack
		push esi                    // param_2  — attacker stack
		call magnetism_try_redirect
		add esp, 0x24

		mov ebx, eax
		pop edi
		pop esi
		xor edi, edi
		jmp[Magnetism_continue]
	}
}
