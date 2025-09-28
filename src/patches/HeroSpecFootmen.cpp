#include "pch.h"

// Infantry commander attacks decrease target's morale

void InfantryCommanderFork();

int InfantryCommander_fork = 0x008CF7F3;
int InfantryCommander_return = 0x008CF81F;
int InfantryCommander_end = 0x008CF875;

void InfantryCommanderTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, InfantryCommander_fork, 44, InfantryCommanderFork, 0, 0, 0 });
}

__declspec(naked) void InfantryCommanderFork() {
	__asm
	{
		mov ecx, dword ptr [esi + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		mov eax, dword ptr [edx + esi + 0x4]
		lea ecx, dword ptr [edx + esi + 0x4]

		push 0x6
		call dword ptr [eax + 0x294]
		test al, al
		je INFANTRY_COMANDER_END
		jmp[InfantryCommander_return]

	INFANTRY_COMANDER_END:
		jmp[InfantryCommander_end]
	}
}
