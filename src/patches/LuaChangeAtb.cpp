#include "pch.h"

// Register combat Lua command changeATB(unit, delta) — adds delta to the unit's current ATB.
// setATB remains vanilla (absolute assignment). Uses the same CSetATBValue command as setATB.

static const char lua_change_atb_name[] = "changeATB";

using StringInitFn = void(__thiscall*)(void* out, char* begin, char* end);
using ReleaseFn = void(__stdcall*)(int ptr);
// Matches lua_setATB @ 0x0056CFCF: ECX=parse output @ frame+0x18, EDX=scriptCtx,
// stack=(float out @ frame+0x24, value type desc @ frame+0x30).
using ReadCombatScriptArgsFn = char(__fastcall*)(void* parseOutput, int scriptCtx, float* valueOut, void* valueTypeDescEndPtr);
using CleanupParseFailFn = void(__fastcall*)(int* parseField0, int* parsedNamePtr);
using GetScriptContextFn = int(__fastcall*)(void* guard);
using RTDynamicCastFn = void*(__cdecl*)(void* ptr, int hint, void* srcType, void* dstType, int isReference);
using GetUnitByNameFn = int(__thiscall*)(int* arena, void* unitNameField);
using EngineAllocFn = int(__stdcall*)(size_t size);
using RefcountStoreFn = void(__thiscall*)(int* slot, int unit);
using CSetATBValueCtorFn = void(__thiscall*)(void* cmd, int* unit, float* value);
using PostCommandFn = void(__thiscall*)(int* arena, void* cmd);
using GetErrorOutputFn = int(__fastcall*)(void* guard);
using FormatMessageFn = int(__cdecl*)(const char* fmt, ...);
using ErrorOutputFn = void(__thiscall*)(int* err, const char* msg);
using CleanupParseFn = void(__fastcall*)(int* parseState);

static float read_unit_atb(int* unit) {
	if (!unit) return 0.f;
	uintptr_t* vtable = *(uintptr_t**)unit;
	auto getAtbInfo = (int*(__thiscall*)(int*))vtable[0x188 / 4];
	int* info = getAtbInfo(unit);
	if (!info) return 0.f;
	return *(float*)((char*)info + 0x1C);
}

static void report_lua_error(void* guard, const char* message) {
	int* err = (int*)((GetErrorOutputFn)0x00B0F610)(guard);
	if (!err) return;
	uintptr_t* errVtable = *(uintptr_t**)err;
	auto output = (ErrorOutputFn)errVtable[1];
	output(err, message);
}

int __fastcall lua_changeATB(int scriptCtx) {
	if (!scriptCtx) return 0;

	char nameTypeDesc[12] = {};
	char argTypeDesc[12] = {};
	((StringInitFn)0x00401B20)(nameTypeDesc, (char*)0x00E1ED94, (char*)0x00E1ED96);
	((StringInitFn)0x00401B20)(argTypeDesc, (char*)0x00E1F688, (char*)0x00E1F68E);

	int parsedNameObj = 0;
	float delta = 0.f;

	if (!((ReadCombatScriptArgsFn)0x00B0F690)(nameTypeDesc, scriptCtx, &parsedNameObj, &delta)) {
		((CleanupParseFn)0x004C2850)((int*)nameTypeDesc);
		return 0;
	}

	struct ScriptGuard {
		void* vtable;
		int zero;
		int ctx;
		char flag;
	} guard = { (void*)0x00E14488, 0, scriptCtx, 0 };

	int rawCtx = ((GetScriptContextFn)0x00B0F650)(&guard);
	int* arena = (int*)((RTDynamicCastFn)0x00D119FC)(
		(void*)rawCtx, 0, (void*)0x00F272C0, (void*)0x00F32224, 0);

	if (!arena) {
		report_lua_error(&guard, "Can't run: can't get combat arena");
		((CleanupParseFn)0x004C2850)((int*)nameTypeDesc);
		return 0;
	}

	uintptr_t* arenaVtable = *(uintptr_t**)arena;
	int* unit = (int*)((GetUnitByNameFn)arenaVtable[0xC / 4])(
		arena, (char*)parsedNameObj + 0xC);

	if (!unit) {
		const char* unitName = *(const char**)((char*)parsedNameObj + 0xC);
		const char* msg = (const char*)((FormatMessageFn)0x00A47FD0)(
			"There is no unit on combat arena with name \"%s\"", unitName);
		report_lua_error(&guard, msg);
		((CleanupParseFn)0x004C2850)((int*)nameTypeDesc);
		return 0;
	}

	delta += read_unit_atb(unit);

	void* cmd = (void*)((EngineAllocFn)0x00878FD0)(0x14);
	if (!cmd) {
		((CleanupParseFn)0x004C2850)((int*)nameTypeDesc);
		return 0;
	}

	int unitRef = (int)unit;
	((RefcountStoreFn)0x004DD6E0)(&unitRef, (int)unit);
	((CSetATBValueCtorFn)0x00566800)(cmd, &unitRef, &delta);
	((PostCommandFn)arenaVtable[1])(arena, cmd);

	((CleanupParseFn)0x004C2850)((int*)nameTypeDesc);
	return 0;
}

void LuaChangeAtb_init(pugi::xml_document& doc) {
	// Replace RemoveAllUnits @ 0x00F32F98 (lua_removeAllUnits @ 0x0056B400) with changeATB.
	assembly_patches.push_back({ PATCH_INT, 0x00F32F98, 4, nullptr, (int)(uintptr_t)lua_change_atb_name, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00F32F9C, 4, nullptr, (int)(uintptr_t)lua_changeATB, 0, 0, 0 });
}
