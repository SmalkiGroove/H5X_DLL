#include "pch.h"

// EDIT TOWER DAMAGE CALCULATION

void TowerFork();

int Tower_fork = 0x00CD221D;
int Tower_return = 0x00CD2222;

void TowerDamage_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Tower_fork, 5, TowerFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, Tower_return, 9, nullptr, 0, 0, 0, "5F5E83C40CC3CCCCCC" });
	assembly_patches.push_back({ PATCH_INT, 0x00BFE043, 4, nullptr, 200, 0, 0, 0 }); // Main tower damages
	assembly_patches.push_back({ PATCH_INT, 0x00BFE04D, 4, nullptr, 140, 0, 0, 0 }); // Side tower damages
}

__declspec(naked) void TowerFork() {
	__asm
	{
		sub esp, 0x4
		fild dword ptr [esp + 0xC]
		fidiv dword ptr [esp + 0x10]
		fmul dword ptr [constf_0_5]
		fstp dword ptr [esp]
		fild dword ptr [esp + 0xC]
		fimul dword ptr [esp + 0xC]
		fidiv dword ptr [esp + 0x10]
		fidiv dword ptr [esp + 0x10]
		fmul dword ptr [constf_0_5]
		fadd dword ptr [esp]
		add esp, 0x4
		jmp[Tower_return]
	}
}
