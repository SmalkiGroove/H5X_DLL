#include "pch.h"

// CALL OF THE NETHERWORLD SHOULD NOT HIT ORCS AND WAR MACHINES

void UnholyWordImmunitiesFork();

int UnholyWordImmunities_fork = 0x00977643;
int UnholyWordImmunities_return = 0x00977649;
int UnholyWordImmunities_end = 0x0097763F;

void UnholyWordImmunitiesFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, UnholyWordImmunities_fork, 6, UnholyWordImmunitiesFork, 0, 0, 0 });
}

__declspec(naked) void UnholyWordImmunitiesFork() {
	__asm
	{
		mov edx, dword ptr [esi]
		push 0x69 // demon blood ability
		mov ecx, esi
		call dword ptr [edx + 0x28C]
		test al, al
		jne FUNCTION_RETURN

		mov edx, dword ptr [esi]
		push 0x9 // mechanical ability
		mov ecx, esi
		call dword ptr[edx + 0x28C]
		test al, al
		jne FUNCTION_RETURN

		mov edx, dword ptr [esi]
		push 0xA
		mov ecx, esi
		jmp[UnholyWordImmunities_return]

	FUNCTION_RETURN :
		jmp[UnholyWordImmunities_end]
	}
}
