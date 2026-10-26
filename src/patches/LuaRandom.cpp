#include "pch.h"
#include "LuaScript.h"

// LUA FUNCTION random(a, b): random integer in [min(a, b), max(a, b)], adventure map and combat scripts
// - Adventure map: replaces the handler of the vanilla advmap random(n) entry (lua_random 0x005DFC30). The value
//   comes from the script owner's generator, IAdventureMap vtable +0x0C(n) -> [0, n), as vanilla: the adventure
//   screen's in single player, the world's in multiplayer / hotseat (CWorldScriptSystem, every machine runs the same
//   script on its own world, so the values stay in sync).
// - Combat: registered in the H5X combat table (LuaCombatFunctions.cpp).
//   The combat's own generator (CCombat vtable +0x0C -> IRandomGenerator, also used by luck / morale / ability rolls)
//   is not advanced: the first call of each combat copies its state and the script draws from that copy with the
//   same algorithm (IRandomGenerator vtable +0x10, 0x008C0470), so script rolls never shift the battle's own rolls.
// Arguments are rounded to integers. Errors go through the engine's script error.

int LuaAdvmapRandom_entry = 0x00F35140;          // { "random", lua_random 0x005DFC30 } in the advmap game table

void LuaRandom_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, LuaAdvmapRandom_entry + 4, 4, nullptr, (int)(uintptr_t)lua_random, 0, 0, 0 });
}

static int* random_combat = nullptr;     // combat the copy was taken from (only compared)
static unsigned int random_state = 0;    // copy of its generator state (IRandomGenerator +0x08)

// Same generator as IRandomGenerator vtable +0x10 (0x008C0470): [0, range)
static unsigned int next_random(unsigned int* state, unsigned int range) {
	unsigned int first = *state * 0x343FD + 0x269EC3;
	unsigned int second = first * 0x343FD + 0x269EC3;
	*state = second;
	return (unsigned int)(((int)first >> 7) ^ (int)(second << 6)) % range;
}

unsigned int combat_script_random(int* combat, unsigned int range) {
	if (combat != random_combat) {
		int* generator = ((ThiscallIntPtr_IntPtr)(*(int***)combat)[0x0C / 4])(combat);
		random_combat = combat;
		random_state = (unsigned int)generator[0x08 / 4];
	}
	return next_random(&random_state, range);
}

int __fastcall lua_random(int* state) {
	if (!state) return 0;
	int a, b;
	if (!lua_number_arg(state, 1, &a) || !lua_number_arg(state, 2, &b)) {
		lua_error(state, "random: two numbers expected, random(a, b)");
		return 0;
	}
	if (a == b) {
		lua_push_number(state, (double)a);
		return 1;
	}
	int low = a < b ? a : b;
	unsigned int range = (unsigned int)((a < b ? b : a) - low) + 1;
	int value = 0;
	int* map = lua_owner(state, RTTI_ADVENTURE_MAP);
	int* arena = map ? nullptr : lua_owner(state, RTTI_COMBAT_ARENA);
	if (map) {
		value = ((ThiscallInt_IntPtr_Int)(*(int***)map)[0x0C / 4])(map, (int)range);
	}
	else if (arena) {
		int* combat = ((ThiscallIntPtr_IntPtr)(*(int***)arena)[0x08 / 4])(arena);
		value = (int)combat_script_random(combat, range);
	}
	else {
		lua_error(state, "random: can't get adventure map or combat arena");
		return 0;
	}
	lua_push_number(state, (double)(low + value));
	return 1;
}
