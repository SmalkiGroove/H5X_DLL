#pragma once
#ifndef SCRIPT_H
#define SCRIPT_H

// Lua script engine objects. RE rule: .cursor/rules/h5x-re-lua-engine.mdc.
#include "types/types.h"

typedef void(__thiscall* ScriptError)(int*, const char*);
typedef void(__thiscall* RunScript)(int*, const char*);

// NScript::CLuaScriptEngine (vtable 0x00E93760), from 0x00B0F610 (call context -> engine).
enum {
	ScriptEngine_vtable_addr = 0x00E93760
};

struct ScriptEngine_vtable {
	int* call_0;
	ScriptError error; // 0x04 (4) - 0x00CAB4E0, thiscall(message): script error, stops the calling thread
};

struct IScriptEngine {
	ScriptEngine_vtable* instance;

	void error(const char* message) {
		instance->error((int*)this, message);
	}
};

// Combat script runner (combat manager +0x1B4). Concrete vtable not identified.
struct ScriptRunner_vtable {
	RunScript run; // 0x00 (0) - thiscall(Lua code): runs the code in the combat script engine, no wait
};

struct IScriptRunner {
	ScriptRunner_vtable* instance;

	void run(const char* code) {
		instance->run((int*)this, code);
	}
};

#endif
