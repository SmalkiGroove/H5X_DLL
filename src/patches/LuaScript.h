#pragma once

// Lua 4 script engine helpers for H5X Lua functions (rule: h5x-re-lua-engine.mdc).
// A Lua function is int __fastcall handler(int* state): ECX = Lua state, returns the number of pushed results.

static const int RTTI_ADVENTURE_MAP = 0x00F34FCC;   // NAdventureMapScript::IAdventureMap
static const int RTTI_COMBAT_ARENA = 0x00F32224;    // NCombatArenaScript::ICombatArenaCommon

struct LuaFunction {
	const char* name;
	void* function;
};

// Owner of the script engine (execution context) cast to the given interface, nullptr if it isn't one.
int* lua_owner(int* state, int type_descriptor);
// Script error with a printf format; throws out of the Lua call (no C++ object may need destruction in the caller).
void lua_error(int* state, const char* format, ...);
// Argument index (1-based): number (rounded) or string, false / nullptr when missing or of another type.
bool lua_number_arg(int* state, int index, int* value);
const char* lua_string_arg(int* state, int index);
bool lua_has_arg(int* state, int index);
void lua_push_number(int* state, double value);
void lua_push_bool(int* state, bool value);      // 1 or nil
// Engine object deleted flag (0x80 at byte +0x0B of the virtual base, like 0x004E9280).
bool engine_object_released(int* object);

int __fastcall lua_random(int* state);             // LuaRandom.cpp
