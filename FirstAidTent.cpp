#include "pch.h"

// EDIT FIRST AID TENT DATA

void FirstAidTentFork();

void FirstAidTent_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x009771A6, 4, nullptr, 20, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771B4, 4, nullptr, 40, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C2, 4, nullptr, 60, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D0, 4, nullptr, 80, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C8, 4, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D6, 4, nullptr, 0, 0, 0, 0 });
	//assembly_patches.push_back({ PATCH_HOOK, 0x009771F0, 6, FirstAidTentFork, 0, 0, 0 });
}

__declspec(naked) void FirstAidTentFork() {
	__asm
	{
		// TODO
	}
}
