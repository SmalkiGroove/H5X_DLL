#include "pch.h"
#include "LuaScript.h"

// LUA: H5X ADVENTURE MAP SCRIPT FUNCTIONS
// The advmap context's pointer to the tutorial helper table (0x00F3E004, IsTutorialItemEnabled ... IsTutorialEnabled,
// shared with the other contexts) is replaced by H5X_ADVMAP_FUNCTIONS, which repeats those six entries.
// - GiveResource(player, resource, amount) / TakeAwayResource(player, resource, amount): add / remove a resource
//   (0 wood ... 6 gold) with one NWorld::CAddPlayerMoneyCmd (vtable 0x00E0DAD8, 0x2C bytes, ctor 0x004E9410:
//   thiscall(&player, int deltas[7]); execute 0x00427350: player resources (IPlayer vtable +0xD8) + deltas, every
//   resource clamped at 0 (0x009D0770), written back with +0xDC). Unlike GetPlayerResource + SetPlayerResource (an
//   absolute CSetPlayerMoneyCmd, executed with the next world segment), concurrent calls add up instead of
//   overwriting each other. Posted like SetPlayerResource (IAdventureMap vtable +0x04): executed with the next
//   segment in single player, at once by the world script system in multiplayer / hotseat.

int __fastcall lua_GiveResource(int* state);
int __fastcall lua_TakeAwayResource(int* state);

static const LuaFunction H5X_ADVMAP_FUNCTIONS[] = {
	{ "IsTutorialItemEnabled", (void*)0x00773C00 },     // the vanilla table this one replaces
	{ "TutorialActivateHint", (void*)0x00773DA0 },
	{ "TutorialSetBlink", (void*)0x00773EE0 },
	{ "TutorialMessageBox", (void*)0x00774020 },
	{ "IsTutorialMessageBoxOpen", (void*)0x00774160 },
	{ "IsTutorialEnabled", (void*)0x007742F0 },
	{ "GiveResource", lua_GiveResource },
	{ "TakeAwayResource", lua_TakeAwayResource },
	{ nullptr, nullptr }
};

static const int lua_tutorial_table = 0x00F3E004;

void LuaAdvmapFunctions_init(pugi::xml_document& doc) {
	lua_replace_context_table("advmap", lua_tutorial_table, H5X_ADVMAP_FUNCTIONS);
}

typedef void(__thiscall* AddMoneyCmdCtorFn)(int*, int**, int*);
static AddMoneyCmdCtorFn add_money_cmd_ctor = (AddMoneyCmdCtorFn)0x004E9410;

static int** vtable(int* object) {
	return *(int***)object;
}

// sign = 1 to give, -1 to take away
static int change_resource(int* state, const char* function, int sign) {
	int player_id, resource, amount;
	if (!lua_number_arg(state, 1, &player_id) || !lua_number_arg(state, 2, &resource) || !lua_number_arg(state, 3, &amount)) {
		lua_error(state, "%s: three numbers expected (player, resource, amount)", function);
		return 0;
	}
	if (player_id < 1 || player_id > 8) {
		lua_error(state, "%s: player ID must be 1 through 8", function);
		return 0;
	}
	if (resource < 0 || resource > 6) {
		lua_error(state, "%s: invalid resource %i", function, resource);
		return 0;
	}
	if (amount < 0) {
		lua_error(state, "%s: amount cannot be negative", function);
		return 0;
	}
	int* map = lua_owner(state, RTTI_ADVENTURE_MAP);
	if (!map) {
		lua_error(state, "%s: can't get adventure map", function);
		return 0;
	}
	int* world = ((ThiscallIntPtr_IntPtr)vtable(map)[0x00])(map);
	typedef int*(__thiscall* GetPlayerFn)(int*, int);
	int* player = ((GetPlayerFn)vtable(world)[0x68 / 4])(world, player_id);   // CWorld vtable +0x68: IPlayer
	if (!player || engine_object_released(player) || ((ThiscallInt_IntPtr)vtable(player)[0x6C / 4])(player) == 0) {
		lua_error(state, "%s: player %i doesn't take part in the current game", function, player_id);
		return 0;
	}
	if (amount == 0) return 0;
	int deltas[7] = { 0, 0, 0, 0, 0, 0, 0 };
	deltas[resource] = sign * amount;
	int* command = (int*)engine_alloc(0x2C);
	if (!command) return 0;
	add_money_cmd_ctor(command, &player, deltas);
	((ThiscallVoid_IntPtr_IntPtr)vtable(map)[0x04 / 4])(map, command);
	return 0;
}

int __fastcall lua_GiveResource(int* state) {
	if (!state) return 0;
	return change_resource(state, "GiveResource", 1);
}

int __fastcall lua_TakeAwayResource(int* state) {
	if (!state) return 0;
	return change_resource(state, "TakeAwayResource", -1);
}
