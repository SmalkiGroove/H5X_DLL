#include "pch.h"

// COMBAT SCRIPTS IN MULTIPLAYER: LOADED ON ONE MACHINE ONLY
// Vanilla gates every script context with func_IsTriggerScriptHost (0x006B9DE0): true when the world params (CWorld
// vtable +0x14 -> &+0x88) have neither the multiplayer (+0x88) nor the hotseat (+0x89) flag, i.e. single player.
// The adventure map, trigger event, town and flyby gates keep the vanilla behaviour: outside single player the map
// script runs in the world's own script system (CWorld +0x1D0, loader 0x00992D00), which fires the triggers.
// Combat script manager constructor (0x0054C2F0, gate at 0x0054CE2A): the combat script is loaded (+0x4F0 = 1,
// func_LoadCombatScript 0x0053E810) in single player and hotseat (one machine), and in network multiplayer only on the
// machine of the player who owns the script side, since every machine running it plays its effects again:
// - the attacker (Lua side 0) when its army belongs to a human player (IPlayer vtable +0x188 host type, 0 = HUMAN),
// - else the defender when it does,
// - else (no human side) every machine, as before.
// Side -> army: CCombat (manager +0x2A0) vtable +0x84(1 for side 0, 0 for side 1) as in lua_GetHost (0x00567000),
// army vtable +0x04 -> IPlayer (0 for neutrals), IPlayer vtable +0x2C = player id.
// The local player is the adventure screen's (+0x3C, the player of its resource bar), captured with the params of its
// world (+0x18) at the end of the adventure screen constructor (0x006089A0). A combat of another world (no adventure
// screen) also loads the script on every machine.

void CombatScriptHostFork();
void AdvMapLocalPlayerFork();

int CombatScriptHost_fork = 0x0054CE2A;      // call func_IsTriggerScriptHost / test al,al / je 0x0054CE52
int CombatScriptHost_return = 0x0054CE33;    // load the combat script
int CombatScriptHost_skip = 0x0054CE52;      // "Warning: scripts are not allowed"
int AdvMapLocalPlayer_fork = 0x00608D08;     // mov ecx,[esi+18h] / mov edx,[ecx]
int AdvMapLocalPlayer_return = 0x00608D0D;   // call [edx+0ACh]

void Scripts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, CombatScriptHost_fork, 9, CombatScriptHostFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, AdvMapLocalPlayer_fork, 5, AdvMapLocalPlayerFork, 0, 0, 0 });
}

static char* local_world_params = nullptr;   // params (CWorld +0x88) of the last adventure screen's world, only compared
static int local_player_id = -1;             // its local player

static char* world_params(int* world) {
	return (char*)((ThiscallIntPtr_IntPtr)(*(int***)world)[0x14 / 4])(world);
}

static int player_id(int* player) {
	return ((ThiscallInt_IntPtr)(*(int***)player)[0x2C / 4])(player);
}

static bool is_human(int* player) {
	return ((ThiscallInt_IntPtr)(*(int***)player)[0x188 / 4])(player) == 0;
}

// IPlayer owning a combat side (0 attacker, 1 defender), nullptr for a neutral or missing army.
static int* side_player(int* combat, int side) {
	typedef int*(__thiscall* GetArmyFn)(int*, int);
	int* army = ((GetArmyFn)(*(int***)combat)[0x84 / 4])(combat, side == 0 ? 1 : 0);
	if (!army) return nullptr;
	int vbase = (*(int**)(army + 1))[1];
	if (*((unsigned char*)army + vbase + 0xB) & 0x80) return nullptr;   // released object (0x004E9280)
	return ((ThiscallIntPtr_IntPtr)(*(int***)army)[0x04 / 4])(army);
}

static void __stdcall capture_local_player(int* screen) {
	int* world = (int*)screen[0x18 / 4];
	int* player = (int*)screen[0x3C / 4];
	local_world_params = world ? world_params(world) : nullptr;
	local_player_id = player ? player_id(player) : -1;
	writeLog(INFO, "CombatScriptHost: adventure screen local player " + std::to_string(local_player_id));
}

// params = world params of the combat (CWorld +0x88), manager = combat script manager
static bool __stdcall combat_script_allowed(char* params, int* manager) {
	if (!params[0]) return true;   // single player or hotseat
	if (local_player_id < 0 || params != local_world_params) {
		writeLog(INFO, "CombatScriptHost: no local player for this world, the combat script is loaded");
		return true;
	}
	int* combat = *(int**)((char*)manager + 0x2A0);
	int* host = side_player(combat, 0);
	if (!host || !is_human(host)) host = side_player(combat, 1);
	if (!host || !is_human(host)) {
		writeLog(INFO, "CombatScriptHost: no human side, the combat script is loaded");
		return true;
	}
	int host_id = player_id(host);
	bool allowed = host_id == local_player_id;
	writeLog(INFO, "CombatScriptHost: script player " + std::to_string(host_id) + ", local player "
		+ std::to_string(local_player_id) + (allowed ? ", the combat script is loaded" : ", no combat script"));
	return allowed;
}

// esi = adventure screen (+0x18 world, +0x3C local player)
__declspec(naked) void AdvMapLocalPlayerFork() {
	__asm
	{
		push esi
		call capture_local_player
		mov ecx, dword ptr [esi + 0x18]
		mov edx, dword ptr [ecx]
		jmp [AdvMapLocalPlayer_return]
	}
}

// esi = combat script manager, ecx = world params
__declspec(naked) void CombatScriptHostFork() {
	__asm
	{
		push esi
		push ecx
		call combat_script_allowed
		test al, al
		je SKIP
		jmp [CombatScriptHost_return]
	SKIP:
		jmp [CombatScriptHost_skip]
	}
}
