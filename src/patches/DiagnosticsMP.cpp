#include "pch.h"

// TEMPORARY DIAGNOSTICS: multiplayer script execution (H5.log, "[MP]" lines), to be removed after the test
// Logs, with the time (GetTickCount), on each machine:
// - the adventure world seen by lua_SetTrigger (0x005DD765) and, once per world, its params block (+0x84..+0xC4,
//   multiplayer / hotseat flags at +0x88 / +0x89) and the 8 players (vtable +0x184 is human, +0x188 host type, raw
//   bytes +0x00..+0x80: the bytes that differ between machines for the same player give the local player),
// - CWorld::FireTrigger calls (0x009902B0) with the world flags and the branch taken,
// - the trigger Lua actually run: directly by CWorldScriptSystem (0x009900FC) or by the display event (0x00607CC5),
// - CSetTriggerActionCmd executions (0x00427B90): where world commands are executed,
// - the waited script calls (0x00CAD110: DoPrepare / DoStart / UnitMove; DoPrepare also dumps the players again)
//   and UnitDeath (0x0056FB47),
// - the "turn pending" byte (combat manager +0x540) set by the display loop: active unit changed (0x00544712) or
//   previous active unit's ATB < 1 / dead (0x00544754); each set leads to one UnitMove call,
// - the adventure map script loader (0x00992D00) calls, each Lua SetTrigger call (type),
// - func_CombatTurn calls (0x0056FEF0: unit, "play it normally" flag), which run UnitMove.

void DiagCaptureWorldFork();
void DiagFireTriggerFork();
void DiagTriggerDirectFork();
void DiagTriggerEventFork();
void DiagSetTriggerCmdFork();
void DiagWaitedScriptFork();
void DiagUnitDeathFork();
void DiagTurnChangedFork();
void DiagTurnAtbFork();
void DiagMapScriptLoadFork();
void DiagCombatTurnFork();

int DiagCaptureWorld_fork = 0x005DD765;     // lua_SetTrigger: mov eax,[ebx] / mov ecx,ebx / call [eax] (world)
int DiagCaptureWorld_return = 0x005DD76B;
int DiagFireTrigger_fork = 0x009902B0;      // func_World_FireTrigger: push esi / mov esi,ecx / cmp byte [esi+88h],0
int DiagFireTrigger_return = 0x009902BA;
int DiagTriggerDirect_fork = 0x009900FC;    // func_World_RunTriggerDirect: mov ecx,[esi+0Ch] / mov esi,[esp+4] (code)
int DiagTriggerDirect_return = 0x00990103;
int DiagTriggerEvent_fork = 0x00607CC5;     // func_AdvMap_RunTriggerEvent: mov ecx,[esi+40h] / mov esi,[esp+4] (code)
int DiagTriggerEvent_return = 0x00607CCC;
int DiagSetTriggerCmd_fork = 0x00427B90;    // CSetTriggerActionCmd execute: mov eax,[ecx+0Ch] / test eax,eax / je
int DiagSetTriggerCmd_return = 0x00427B97;
int DiagSetTriggerCmd_null = 0x00427BA8;
int DiagWaitedScript_fork = 0x00CAD110;     // func_ScriptRunThreadWithCallback: sub esp,18h / push esi / mov esi,ecx
int DiagWaitedScript_return = 0x00CAD116;
int DiagUnitDeath_fork = 0x0056FB47;        // func_CombatDeath: mov esi,[esp+8] / mov eax,[ecx] / push esi / call [eax]
int DiagUnitDeath_return = 0x0056FB50;
int DiagTurnChanged_fork = 0x00544712;      // mov byte ptr [esi+540h],1 (active unit changed)
int DiagTurnChanged_return = 0x00544719;
int DiagTurnAtb_fork = 0x00544754;          // mov byte ptr [esi+540h],1 (previous active unit ATB < 1 or dead)
int DiagTurnAtb_return = 0x0054475B;
int DiagMapScriptLoad_fork = 0x00992D00;    // map script loader: sub esp,28h / push ebx / push esi
int DiagMapScriptLoad_return = 0x00992D05;
int DiagCombatTurn_fork = 0x0056FEF0;       // func_CombatTurn: sub esp,0Ch / push esi / push edi
int DiagCombatTurn_return = 0x0056FEF5;

void DiagnosticsMP_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, DiagCaptureWorld_fork, 6, DiagCaptureWorldFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagFireTrigger_fork, 10, DiagFireTriggerFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagTriggerDirect_fork, 7, DiagTriggerDirectFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagTriggerEvent_fork, 7, DiagTriggerEventFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagSetTriggerCmd_fork, 7, DiagSetTriggerCmdFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagWaitedScript_fork, 6, DiagWaitedScriptFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagUnitDeath_fork, 9, DiagUnitDeathFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagTurnChanged_fork, 7, DiagTurnChangedFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagTurnAtb_fork, 7, DiagTurnAtbFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagMapScriptLoad_fork, 5, DiagMapScriptLoadFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DiagCombatTurn_fork, 5, DiagCombatTurnFork, 0, 0, 0 });
}

static int* diag_world = nullptr;      // last adventure world seen by lua_SetTrigger
static int* diag_dumped_world = nullptr;

static void diag_log(const std::string& message) {
	writeLog(INFO, "[MP] t=" + std::to_string(GetTickCount()) + " " + message);
}

static std::string diag_hex(const void* address) {
	char buffer[16];
	sprintf_s(buffer, "%08X", (unsigned)(uintptr_t)address);
	return buffer;
}

static bool diag_read_byte(const unsigned char* address, unsigned char* value) {
	__try { *value = *address; return true; }
	__except (EXCEPTION_EXECUTE_HANDLER) { return false; }
}

static int diag_read_int(const void* address) {
	__try { return *(const int*)address; }
	__except (EXCEPTION_EXECUTE_HANDLER) { return 0; }
}

// Hex dump of size bytes, "??" when unreadable.
static std::string diag_bytes(const unsigned char* data, int size) {
	std::string out;
	char buffer[4];
	for (int i = 0; i < size; ++i) {
		unsigned char value = 0;
		if (diag_read_byte(data + i, &value)) sprintf_s(buffer, "%02X", value);
		else strcpy_s(buffer, "??");
		out += buffer;
		if ((i & 3) == 3) out += ' ';
	}
	return out;
}

static std::string diag_engine_string(char** string) {
	if (!string || !string[0] || string[1] < string[0] || string[1] - string[0] > 256) return "<none>";
	return std::string(string[0], string[1]);
}

// Player id 1..8 of the world (CWorld vtable +0x68), nullptr when absent.
static int* diag_player(int* world, int id) {
	typedef int*(__thiscall* GetPlayerFn)(int*, int);
	int* player = nullptr;
	__try { player = ((GetPlayerFn)(*(int***)world)[0x68 / 4])(world, id); }
	__except (EXCEPTION_EXECUTE_HANDLER) { player = nullptr; }
	return player;
}

static int diag_player_slot(int* player, int offset) {
	typedef int(__thiscall* SlotFn)(int*);
	int value = -1;
	__try { value = ((SlotFn)(*(int***)player)[offset / 4])(player); }
	__except (EXCEPTION_EXECUTE_HANDLER) { value = -1; }
	return value;
}

static void diag_dump_world(int* world, const char* when) {
	if (!world) {
		diag_log(std::string("dump (") + when + "): no adventure world captured");
		return;
	}
	unsigned char* base = (unsigned char*)world;
	diag_log(std::string("dump (") + when + ") world " + diag_hex(world) + " params +84: " + diag_bytes(base + 0x84, 0x40));
	int* script_system = (int*)(uintptr_t)diag_read_int(base + 0x1D0);
	std::string runner = "?";
	if (script_system) runner = diag_bytes((unsigned char*)script_system + 0x08, 8);
	diag_log("  script system " + diag_hex(script_system) + " (+08 world, +0C runner): " + runner);
	for (int id = 1; id <= 8; ++id) {
		int* player = diag_player(world, id);
		if (!player) continue;
		diag_log("  player " + std::to_string(id) + " " + diag_hex(player)
			+ " human(+184)=" + std::to_string(diag_player_slot(player, 0x184) & 0xFF)
			+ " host(+188)=" + std::to_string(diag_player_slot(player, 0x188))
			+ " bytes: " + diag_bytes((unsigned char*)player, 0x80));
	}
}

static void __stdcall diag_capture_world(int* world, int type) {
	diag_log("lua SetTrigger call, type " + std::to_string(type) + ", world " + diag_hex(world));
	diag_world = world;
	if (world != diag_dumped_world) {
		diag_dumped_world = world;
		diag_dump_world(world, "advmap script");
	}
}

static void __stdcall diag_fire_trigger(int* world, char** name) {
	unsigned char* base = (unsigned char*)world;
	int* script_system = *(int**)(base + 0x1D0);
	bool queued = base[0x88] == 0 && base[0x89] == 0;
	diag_log("FireTrigger world " + diag_hex(world) + " name=" + diag_engine_string(name)
		+ " flags88/89=" + std::to_string(base[0x88]) + "/" + std::to_string(base[0x89])
		+ " flag84=" + std::to_string(base[0x84])
		+ (queued ? " -> event queued" : (script_system ? " -> direct (script system)" : " -> dropped (no script system)")));
}

static void __stdcall diag_trigger_direct(const char* code) {
	diag_log(std::string("trigger RUN direct: ") + (code ? code : "<null>"));
}

static void __stdcall diag_trigger_event(const char* code) {
	diag_log(std::string("trigger RUN event: ") + (code ? code : "<null>"));
}

static void __stdcall diag_set_trigger_cmd(int* command) {
	diag_log("CSetTriggerActionCmd executed: trigger " + diag_hex(*(int**)((char*)command + 0x0C))
		+ " name=" + diag_engine_string((char**)((char*)command + 0x10)));
}

static void __stdcall diag_waited_script(int* runner, const char* code) {
	int attached = diag_read_int((char*)runner + 0x1C);
	diag_log("waited script " + diag_hex(runner) + " (+1C=" + diag_hex((void*)(uintptr_t)attached) + "): "
		+ (code ? code : "<null>"));
	if (code && strncmp(code, "DoPrepare", 9) == 0) diag_dump_world(diag_world, "combat DoPrepare");
}

static void __stdcall diag_unit_death(const char* code) {
	diag_log(std::string("script: ") + (code ? code : "<null>"));
}

static void __stdcall diag_turn_pending(int reason) {
	diag_log(reason ? "turn pending: previous active unit ATB < 1 or dead" : "turn pending: active unit changed");
}

static void __stdcall diag_combat_turn(int* manager, int* unit, int play_normally) {
	diag_log("func_CombatTurn manager " + diag_hex(manager) + " unit " + diag_hex(unit) + " play normally=" + std::to_string(play_normally & 0xFF));
}

static void __stdcall diag_map_script_load(int* loader, int* script) {
	diag_log("map script loader " + diag_hex(loader) + " script object " + diag_hex(script));
}

// ebx = IAdventureMap; replays the call returning the world (eax)
__declspec(naked) void DiagCaptureWorldFork() {
	__asm
	{
		mov eax, dword ptr [ebx]
		mov ecx, ebx
		call dword ptr [eax]
		pushad
		push ebp
		push eax
		call diag_capture_world
		popad
		jmp [DiagCaptureWorld_return]
	}
}

// ecx = world, [esp+4] = name string, [esp+8] = params
__declspec(naked) void DiagFireTriggerFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x24]
		push ecx
		call diag_fire_trigger
		popad
		push esi
		mov esi, ecx
		cmp byte ptr [esi + 0x88], 0
		jmp [DiagFireTrigger_return]
	}
}

// [esp+4] = Lua code
__declspec(naked) void DiagTriggerDirectFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x24]
		call diag_trigger_direct
		popad
		mov ecx, dword ptr [esi + 0x0C]
		mov esi, dword ptr [esp + 4]
		jmp [DiagTriggerDirect_return]
	}
}

__declspec(naked) void DiagTriggerEventFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x24]
		call diag_trigger_event
		popad
		mov ecx, dword ptr [esi + 0x40]
		mov esi, dword ptr [esp + 4]
		jmp [DiagTriggerEvent_return]
	}
}

// ecx = command
__declspec(naked) void DiagSetTriggerCmdFork() {
	__asm
	{
		pushad
		push ecx
		call diag_set_trigger_cmd
		popad
		mov eax, dword ptr [ecx + 0x0C]
		test eax, eax
		je NULL_TRIGGER
		jmp [DiagSetTriggerCmd_return]
	NULL_TRIGGER:
		jmp [DiagSetTriggerCmd_null]
	}
}

// ecx = runner, [esp+4] = Lua code
__declspec(naked) void DiagWaitedScriptFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x24]
		push ecx
		call diag_waited_script
		popad
		sub esp, 0x18
		push esi
		mov esi, ecx
		jmp [DiagWaitedScript_return]
	}
}

// ecx = runner, [esp+8] = Lua code "UnitDeath(...)"
__declspec(naked) void DiagUnitDeathFork() {
	__asm
	{
		mov esi, dword ptr [esp + 8]
		pushad
		push esi
		call diag_unit_death
		popad
		mov eax, dword ptr [ecx]
		push esi
		call dword ptr [eax]
		jmp [DiagUnitDeath_return]
	}
}

__declspec(naked) void DiagTurnChangedFork() {
	__asm
	{
		mov byte ptr [esi + 0x540], 1
		jmp [DiagTurnChanged_return]
	}
}

__declspec(naked) void DiagTurnAtbFork() {
	__asm
	{
		pushad
		push 1
		call diag_turn_pending
		popad
		mov byte ptr [esi + 0x540], 1
		jmp [DiagTurnAtb_return]
	}
}

// ecx = loader object, [esp+4] = map script object (or 0)
__declspec(naked) void DiagMapScriptLoadFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x24]
		push ecx
		call diag_map_script_load
		popad
		sub esp, 0x28
		push ebx
		push esi
		jmp [DiagMapScriptLoad_return]
	}
}

// ecx = combat script manager, [esp+4] = unit, [esp+8] = "play the turn normally" flag
__declspec(naked) void DiagCombatTurnFork() {
	__asm
	{
		pushad
		push dword ptr [esp + 0x28]
		push dword ptr [esp + 0x28]
		push ecx
		call diag_combat_turn
		popad
		sub esp, 0x0C
		push esi
		push edi
		jmp [DiagCombatTurn_return]
	}
}
