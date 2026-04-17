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
		// crystal scales
		mov eax, dword ptr [esi]
		push 0x42
		mov ecx, esi
		call dword ptr [eax + 0x28C]
		test al, al
		jne FUNCTION_END

		// mechanical
		mov eax, dword ptr [esi]
		push 0x9
		mov ecx, esi
		call dword ptr [eax + 0x28C]
		test al, al
		jne FUNCTION_END

		// dragon flame tongue
		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax + 0xC]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
		test eax, eax
		je FUNCTION_RETURN
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0x74]
		mov ecx, eax
		push 0x2B
		call[count_equipped_artifact]
		test eax, eax
		jne FUNCTION_END

	FUNCTION_RETURN:
		mov edx, dword ptr [esi]
		mov ecx, esi
		call dword ptr [edx + 0x1A8]
		jmp[ColdDeath_return]

	FUNCTION_END:
		jmp[ColdDeath_end]
	}
}
