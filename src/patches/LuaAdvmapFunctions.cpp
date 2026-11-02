#include "pch.h"
#include "LuaScript.h"
#include "structs/AdvMap.h"

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
// - AddObjectDwellingCreatures(object, creature, delta): adds delta (negative to remove) to a town / dwelling's
//   recruits, at least 0. The exe has no additive dwelling command: SetObjectDwellingCreatures (0x005CD270) posts a
//   NWorld::CSetDwellingCreaturesCmd (vtable 0x00E0D6C4, 0x18 bytes, ctor 0x005D5090: thiscall(&dwelling, &creature,
//   &count), only built there, count checked >= 0) whose execute (0x00427C00) sets the absolute count through the
//   dwelling's vtable +0xFC(creature, count); two threads doing Get + Set in the same segment overwrite each other.
//   This function posts the same command with a negative count, the delta tagged by bit 31 (|delta| < 2^30), and the
//   execute's set call (0x00427C2B) adds a tagged delta to the current count (vtable +0x100(creature)) when the
//   command runs. The count travels with the command, so multiplayer machines compute the same result.

int __fastcall lua_GiveResource(int* state);
int __fastcall lua_TakeAwayResource(int* state);
int __fastcall lua_AddObjectDwellingCreatures(int* state);
void __fastcall set_dwelling_creatures(IDwelling* dwelling, int, int creature, int count);

static const LuaFunction H5X_ADVMAP_FUNCTIONS[] = {
	{ "IsTutorialItemEnabled", (void*)0x00773C00 },     // the vanilla table this one replaces
	{ "TutorialActivateHint", (void*)0x00773DA0 },
	{ "TutorialSetBlink", (void*)0x00773EE0 },
	{ "TutorialMessageBox", (void*)0x00774020 },
	{ "IsTutorialMessageBoxOpen", (void*)0x00774160 },
	{ "IsTutorialEnabled", (void*)0x007742F0 },
	{ "GiveResource", lua_GiveResource },
	{ "TakeAwayResource", lua_TakeAwayResource },
	{ "AddObjectDwellingCreatures", lua_AddObjectDwellingCreatures },
	{ nullptr, nullptr }
};

static const int lua_tutorial_table = 0x00F3E004;
int DwellingCmdSet_call = 0x00427C2B;   // call [edx+0xFC] in CSetDwellingCreaturesCmd execute

void LuaAdvmapFunctions_init(pugi::xml_document& doc) {
	lua_replace_context_table("advmap", lua_tutorial_table, H5X_ADVMAP_FUNCTIONS);
	assembly_patches.push_back({ PATCH_CALL, DwellingCmdSet_call, 6, set_dwelling_creatures, 0, 0, 0 });
}

typedef void(__thiscall* AddMoneyCmdCtorFn)(int*, int**, int*);
static AddMoneyCmdCtorFn add_money_cmd_ctor = (AddMoneyCmdCtorFn)0x004E9410;

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
	IAdventureMap* map = (IAdventureMap*)lua_owner(state, RTTI_ADVENTURE_MAP);
	if (!map) {
		lua_error(state, "%s: can't get adventure map", function);
		return 0;
	}
	IPlayer* player = map->world()->player(player_id);
	if (!player || engine_object_released((int*)player) || !player->is_active()) {
		lua_error(state, "%s: player %i doesn't take part in the current game", function, player_id);
		return 0;
	}
	if (amount == 0) return 0;
	int deltas[7] = { 0, 0, 0, 0, 0, 0, 0 };
	deltas[resource] = sign * amount;
	int* command = (int*)engine_alloc(0x2C);
	if (!command) return 0;
	int* player_ref = (int*)player;
	add_money_cmd_ctor(command, &player_ref, deltas);
	map->post_command(command);
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

typedef void(__thiscall* DwellingCmdCtorFn)(int*, int**, int*, int*);

static DwellingCmdCtorFn dwelling_cmd_ctor = (DwellingCmdCtorFn)0x005D5090;

static const int creature_id_count = 999;           // 0x00ACB380
static const int dwelling_delta_limit = 0x40000000;

static bool engine_object_alive(int* object) {      // 0x004E9280
	return object && !engine_object_released(object);
}

int __fastcall lua_AddObjectDwellingCreatures(int* state) {
	if (!state) return 0;
	const char* function = "AddObjectDwellingCreatures";
	const char* name = lua_string_arg(state, 1);
	int creature, delta;
	if (!name || !lua_number_arg(state, 2, &creature) || !lua_number_arg(state, 3, &delta)) {
		lua_error(state, "%s: object name and two numbers expected (object, creature, delta)", function);
		return 0;
	}
	if (!*name) {
		lua_error(state, "%s: object name is not specified", function);
		return 0;
	}
	if (creature < 0 || creature >= creature_id_count) {
		lua_error(state, "%s: invalid creature ID #%i", function, creature);
		return 0;
	}
	if (delta <= -dwelling_delta_limit || delta >= dwelling_delta_limit) {
		lua_error(state, "%s: delta %i out of range", function, delta);
		return 0;
	}
	IAdventureMap* map = (IAdventureMap*)lua_owner(state, RTTI_ADVENTURE_MAP);
	if (!map) {
		lua_error(state, "%s: can't get adventure map", function);
		return 0;
	}
	IMapObject* object = map->find_object(name);
	int* dwelling = engine_object_alive((int*)object) ? (int*)object->dwelling() : nullptr;
	if (!engine_object_alive(dwelling)) {
		lua_error(state, "%s: town / dwelling \"%s\" doesn't exist", function, name);
		return 0;
	}
	if (delta == 0) return 0;
	int count = (int)((unsigned)delta | 0x80000000u);
	int* command = (int*)engine_alloc(0x18);
	if (!command) return 0;
	dwelling_cmd_ctor(command, &dwelling, &creature, &count);
	map->post_command(command);
	return 0;
}

// replaces the dwelling's set call (thiscall, same stack layout): a negative count is an H5X delta
void __fastcall set_dwelling_creatures(IDwelling* dwelling, int, int creature, int count) {
	if (count < 0) {
		int delta = (int)((unsigned)count << 1) >> 1;
		count = dwelling->creatures(creature) + delta;
		if (count < 0) count = 0;
	}
	dwelling->set_creatures(creature, count);
}
