#include "pch.h"

// COLD DEATH SHOULD NOT APPLY ON MECHANICAL AND CRYSTAL SCALES UNITS

void ColdDeathFork();

int ColdDeath_fork = 0x00977AE2;
int ColdDeath_return = 0x00977AEC;
int ColdDeath_end = 0x00977AF0;

void ColdDeathFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ColdDeath_fork, 10, ColdDeathFork, 0, 0, 0 });
}

__declspec(naked) void ColdDeathFork() {
	__asm
	{
		mov eax, dword ptr [esi]
		push 0x42
		mov ecx, esi
		call dword ptr [eax + 0x28C]
		test al, al
		jne FUNCTION_RETURN

		mov eax, dword ptr [esi]
		push 0x9
		mov ecx, esi
		call dword ptr [eax + 0x28C]
		test al, al
		jne FUNCTION_RETURN

		mov edx, dword ptr [esi]
		mov ecx, esi
		call dword ptr [edx + 0x1A8]

		jmp[ColdDeath_return]
	FUNCTION_RETURN:
		jmp[ColdDeath_end]
	}
}
