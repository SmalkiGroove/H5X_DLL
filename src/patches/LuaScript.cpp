#include "pch.h"
#include "LuaScript.h"

// LUA SCRIPT ENGINE HELPERS (no patch): argument reading, results and errors for the H5X Lua functions.
// Lua stack (state +0x10): +0x0C top, +0x10 base, +0x14 values (16 bytes: tag, ..., value at +8).
// Tags: 1 nil, 2 number (double), 3 string.

typedef int*(__thiscall* ScriptContextFn)(int*);                        // 0x00B0F650 owner / 0x00B0F610 engine
typedef void*(__cdecl* DynamicCastFn)(void*, int, void*, void*, int);    // __RTDynamicCast 0x00D119FC
typedef void(__thiscall* PushNumberFn)(int*, double);                    // 0x004BF900
typedef void(__thiscall* PushNilFn)(int*);                               // 0x004BF8C0
typedef const char*(__fastcall* ToStringFn)(int*, int);                  // 0x004BF800
typedef void(__thiscall* ScriptErrorFn)(int*, const char*);              // engine vtable +0x04

static ScriptContextFn script_owner = (ScriptContextFn)0x00B0F650;
static ScriptContextFn script_engine = (ScriptContextFn)0x00B0F610;
static DynamicCastFn dynamic_cast_object = (DynamicCastFn)0x00D119FC;
static PushNumberFn push_number = (PushNumberFn)0x004BF900;
static PushNilFn push_nil = (PushNilFn)0x004BF8C0;
static ToStringFn to_string = (ToStringFn)0x004BF800;

static void* const rtti_object_base = (void*)0x00F272C0;   // CObjectBase

static char* lua_value(int* state, int index) {
	char* stack = (char*)state[0x10 / 4];
	int slot = *(int*)(stack + 0x10) + index - 1;
	if (slot >= *(int*)(stack + 0x0C)) return nullptr;
	return *(char**)(stack + 0x14) + slot * 16;
}

int* lua_owner(int* state, int type_descriptor) {
	int call_context[4] = { 0, 0, (int)(uintptr_t)state, 0 };   // the call context fields read: +0x08 = state
	int* owner = script_owner(call_context);
	if (!owner) return nullptr;
	return (int*)dynamic_cast_object(owner, 0, rtti_object_base, (void*)(uintptr_t)type_descriptor, 0);
}

void lua_error(int* state, const char* format, ...) {
	int call_context[4] = { 0, 0, (int)(uintptr_t)state, 0 };
	int* engine = script_engine(call_context);
	if (!engine) return;
	char message[256];
	va_list args;
	va_start(args, format);
	vsnprintf(message, sizeof(message), format, args);
	va_end(args);
	((ScriptErrorFn)(*(int***)engine)[0x04 / 4])(engine, message);
}

bool lua_has_arg(int* state, int index) {
	char* value = lua_value(state, index);
	return value && *(int*)value != 1;
}

bool lua_number_arg(int* state, int index, int* value) {
	char* object = lua_value(state, index);
	if (!object || *(int*)object != 2) return false;
	*value = (int)floor(*(double*)(object + 8) + 0.5);
	return true;
}

const char* lua_string_arg(int* state, int index) {
	char* object = lua_value(state, index);
	if (!object || *(int*)object != 3) return nullptr;
	return to_string(state, index);
}

void lua_push_number(int* state, double value) {
	push_number(state, value);
}

void lua_push_bool(int* state, bool value) {
	if (value) push_number(state, 1.0);
	else push_nil(state);
}

bool engine_object_released(int* object) {
	int vbase = (*(int**)(object + 1))[1];
	return (*((unsigned char*)object + vbase + 0xB) & 0x80) != 0;
}
