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
		fild dword ptr ss : [esp + 0xC]
		fidiv dword ptr ss : [esp + 0x10]
		fmul dword ptr ds : 0x00F270C4 // ptr to '0000003F' = 0.5
		fstp dword ptr ss : [esp]
		fild dword ptr ss : [esp + 0xC]
		fimul dword ptr ss : [esp + 0xC]
		fidiv dword ptr ss : [esp + 0x10]
		fidiv dword ptr ss : [esp + 0x10]
		fmul dword ptr ds : 0x00F270C4 // ptr to '0000003F' = 0.5
		fadd dword ptr ss : [esp]
		add esp, 0x4
		jmp[Tower_return]
	}
}
