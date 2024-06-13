#include "pch.h"

// ADD SKILLS TO SUMMON SPECIFIC ELEM TYPES

void ElementalSummonTypeFork();

int ElemWarriors_fork = 0x00861684;
int ElemWarriors_return = 0x00861689;
int ElemWarriors_end = 0x00861696;

int WaterCreatureID = 0x56;
int EarthCreatureID = 0x57;
int AirCreatureID = 0x58;
int WaterSkillID = 0x58;
int EarthSkillID = 0x8B;
int AirSkillID = 0xCB;

void ElementalWarriors_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ElemWarriors_fork, 5, ElementalSummonTypeFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x0086286F, 1, nullptr, 0, 0, 0, "EB" }); // remove 40% bonus to Pyromancy
}

__declspec(naked) void ElementalSummonTypeFork() {
	__asm
	{
		mov edx, dword ptr [ebp]
		mov ebx, EarthCreatureID
		push EarthSkillID
		mov ecx, ebp
		call dword ptr [edx + 0x290]
		test al, al
		jne FUNCTION_RETURN

		mov edx, dword ptr [ebp]
		mov ebx, WaterCreatureID
		push WaterSkillID
		mov ecx, ebp
		call dword ptr [edx + 0x290]
		test al, al
		jne FUNCTION_RETURN

		mov edx, dword ptr [ebp]
		mov ebx, AirCreatureID
		push AirSkillID
		mov ecx, ebp
		call dword ptr [edx + 0x290]
		test al, al
		jne FUNCTION_RETURN

		push edi
		lea edx, dword ptr [esp + 0x1C]
		jmp[ElemWarriors_return]

	FUNCTION_RETURN:
		jmp[ElemWarriors_end]
	}
}
