#include "pch.h"

// Paw Strike has 5x less probability to trigger

void PawStrikeFork();

int PawStrike_fork = 0x00A5D277;
int PawStrike_return = 0x00A5D27F;

void PawStrikeTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PawStrike_fork, 8, PawStrikeFork, 0, 0, 0 });
}

__declspec(naked) void PawStrikeFork() {
	__asm
	{
		mov edx, eax
		push ecx
		mov ecx, 0x5
		cdq
		idiv ecx
		pop ecx
		mov dword ptr [esp + 0x10], eax
		fild dword ptr [esp + 0x10]
		jmp[PawStrike_return]
	}
}
