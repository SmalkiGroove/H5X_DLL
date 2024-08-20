#include "pch.h"

// EDIT FIRST AID TENT BASE HEAL FORMULA
// Base heal = 10/20/30/40
// Bonus per level = 1
// Bonus per def = 3/4/5/6
// Dispel tier 0/1/2/3

void FirstAidTentFork();

int FirstAidHeal_fork = 0x009771F0;
int FirstAidHeal_return = 0x009771FE;

#pragma pack(push, 1)
struct TentMod_BonusHealPerDef {
	uint32_t mastery_none;
	uint32_t mastery_basic;
	uint32_t mastery_advanced;
	uint32_t mastery_expert;
};
#pragma pack(pop)
TentMod_BonusHealPerDef per_def_bonuses;

void FirstAidTent_init(pugi::xml_document& doc) {
	per_def_bonuses.mastery_none = 3;
	per_def_bonuses.mastery_basic = 4;
	per_def_bonuses.mastery_advanced = 5;
	per_def_bonuses.mastery_expert = 6;
	assembly_patches.push_back({ PATCH_INT, 0x009771A6, 4, nullptr, 10, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771B4, 4, nullptr, 20, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C2, 4, nullptr, 30, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D0, 4, nullptr, 40, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771AC, 4, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771BA, 4, nullptr, 0, 0, 0, 1 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C8, 4, nullptr, 0, 0, 0, 2 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D6, 4, nullptr, 0, 0, 0, 3 });
	assembly_patches.push_back({ PATCH_HOOK, 0x009771F0, 14, FirstAidTentFork, 0, 0, 0 });
}

__declspec(naked) void FirstAidTentFork() {
	__asm
	{
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		mov edx, dword ptr [ecx + esi + 0x4]
		lea ecx, dword ptr [ecx + esi + 0x4]

		call dword ptr [edx + 0x19C]
		push eax
		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax]
		mov edx, dword ptr [eax]
		push 0x2
		mov ecx, eax
		call dword ptr [edx + 0x174]
		pop ecx
		mov eax, dword ptr [eax*4 + per_def_bonuses]
		mul ecx
		add dword ptr [edi], eax
		
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		mov edx, dword ptr [ecx + esi + 0x4]
		lea ecx, dword ptr [ecx + esi + 0x4]

		call dword ptr [edx + 0x23C]
		add dword ptr [edi], eax

		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		mov edx, dword ptr [ecx + esi + 0x4]
		lea ecx, dword ptr [ecx + esi + 0x4]

		jmp[FirstAidHeal_return]
	}
}
