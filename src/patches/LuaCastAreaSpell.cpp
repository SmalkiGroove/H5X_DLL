#include "pch.h"
#include "LuaScript.h"

// LUA: UnitCastAreaSpell(unit, spell, x, y) accepts -1 for x and / or y, like SummonCreature
// lua_UnitCastAreaSpell (0x00569E60) casts through FUN_00566940 (validation FUN_00982010, then the cast command
// posted to the arena) and raises "Unit ... can`t cast spell #... at (x,y)" when the cell is refused, which stops the
// script thread. A -1 coordinate now means any column / row: the matching cells of the battlefield (CCombat vtable
// +0x44 = width, height) are tried in a random order (combat script generator, see LuaRandom.cpp) through the same
// vanilla function until one is accepted. When no cell is accepted, nothing is cast and no error is raised.

void LuaCastAreaSpellCall();

int LuaCastAreaSpell_call = 0x0056A027;     // call FUN_00566940 in lua_UnitCastAreaSpell
int LuaCastAreaSpell_func = 0x00566940;     // thiscall(ecx caster, eax target unit = 0; arena, spell, cell*), ret 0Ch

void LuaCastAreaSpell_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_CALL, LuaCastAreaSpell_call, 5, LuaCastAreaSpellCall, 0, 0, 0 });
}

static bool vanilla_cast(int* caster, int target, int* arena, int spell, int* cell) {
	char result;
	__asm
	{
		push cell
		push spell
		push arena
		mov eax, target
		mov ecx, caster
		call [LuaCastAreaSpell_func]
		mov result, al
	}
	return result != 0;
}

static bool __stdcall cast_area_spell(int* caster, int target, int* arena, int spell, int* cell) {
	if (cell[0] != -1 && cell[1] != -1) return vanilla_cast(caster, target, arena, spell, cell);
	int* combat = ((ThiscallIntPtr_IntPtr)(*(int***)arena)[0x08 / 4])(arena);
	typedef void(__thiscall* GridSizeFn)(int*, int*, int*);
	int width = 0, height = 0;
	((GridSizeFn)(*(int***)combat)[0x44 / 4])(combat, &width, &height);
	std::vector<int> cells;   // x * 0x10000 + y
	for (int x = 0; x < width; ++x) {
		if (cell[0] != -1 && cell[0] != x) continue;
		for (int y = 0; y < height; ++y) {
			if (cell[1] != -1 && cell[1] != y) continue;
			cells.push_back(x * 0x10000 + y);
		}
	}
	for (size_t left = cells.size(); left > 0; --left) {
		size_t pick = combat_script_random(combat, (unsigned int)left);
		int candidate = cells[pick];
		cells[pick] = cells[left - 1];
		int target_cell[2] = { candidate / 0x10000, candidate % 0x10000 };
		if (vanilla_cast(caster, target, arena, spell, target_cell)) return true;
	}
	writeLog(INFO, "LuaCastAreaSpell: no valid cell for spell " + std::to_string(spell) + " at ("
		+ std::to_string(cell[0]) + "," + std::to_string(cell[1]) + "), nothing cast");
	return true;
}

// Same convention as FUN_00566940: ecx = caster, eax = target unit, [esp+4] arena, [esp+8] spell, [esp+0Ch] cell
__declspec(naked) void LuaCastAreaSpellCall() {
	__asm
	{
		push dword ptr [esp + 0x0C]
		push dword ptr [esp + 0x0C]
		push dword ptr [esp + 0x0C]
		push eax
		push ecx
		call cast_area_spell
		ret 0x0C
	}
}
