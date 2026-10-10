#include "pch.h"

// COMBAT LUA COMMAND changeATB(unitName, delta)
// Adds delta (positive or negative) to the unit's current ATB; setATB(unitName, value) still assigns.
// Reuses the entry of RemoveAllUnits (never used by the game) in the combat Lua function table.
// changeATB runs the vanilla lua_setATB, so argument parsing, error messages and the posted CSetATBValue
// command are unchanged: a hook in lua_setATB replaces the parsed value by current ATB + delta when
// lua_setATB returns into changeATB. The result is floored at 0; set_atb caps it at 1.25.

void lua_changeATB();
void LuaChangeAtbFork();

static const char lua_change_atb_name[] = "changeATB";

int LuaRemoveAllUnits_entry = 0x00F32F98;  // { name, function } in the combat Lua table
int LuaSetAtb_func = 0x0056CF60;           // lua_setATB
int LuaSetAtb_fork = 0x0056D0F1;           // unit found: push 0x14; call EngineAlloc (CSetATBValue)
int LuaSetAtb_return = 0x0056D0F8;
int LuaChangeAtb_setatb_return = 0;        // return address of lua_setATB when called by lua_changeATB

void LuaChangeAtb_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, LuaRemoveAllUnits_entry, 4, nullptr, (int)(uintptr_t)lua_change_atb_name, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, LuaRemoveAllUnits_entry + 4, 4, nullptr, (int)(uintptr_t)lua_changeATB, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, LuaSetAtb_fork, 7, LuaChangeAtbFork, 0, 0, 0 });
}

static void __fastcall change_atb_value(ICombatUnit* unit, float* value) {
	float atb = unit->atb() + *value;
	*value = atb < 0.0f ? 0.0f : atb;
}

// Lua handler, same convention as lua_setATB: ECX = script state, plain ret.
__declspec(naked) void lua_changeATB() {
	__asm
	{
		call CHANGE_ATB_CALL                    // lua_setATB returns to the ret below
		ret

		CHANGE_ATB_CALL:
		mov eax, dword ptr [esp]
		mov dword ptr [LuaChangeAtb_setatb_return], eax
		jmp [LuaSetAtb_func]
	}
}

// EBP = unit, [esp+0x1C] = value parsed from Lua, [esp+0x54] = lua_setATB return address.
__declspec(naked) void LuaChangeAtbFork() {
	__asm
	{
		mov eax, dword ptr [esp + 0x54]
		cmp eax, dword ptr [LuaChangeAtb_setatb_return]
		jne SET_ATB_ALLOC
		lea edx, dword ptr [esp + 0x1C]
		mov ecx, ebp
		call change_atb_value

		SET_ATB_ALLOC:
		push 0x14
		call [engine_alloc]
		jmp [LuaSetAtb_return]
	}
}
