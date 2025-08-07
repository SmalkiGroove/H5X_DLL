#include "pch.h"

// Temptress gives shooters atb

void TemptressFork();

int Temptress_fork = 0x00A65C35;
int Temptress_return = 0x00A65C4D;
int Temptress_end = 0x00A65CE1;

void TemptressTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Temptress_fork, 24, TemptressFork, 0, 0, 0 });
}

__declspec(naked) void TemptressFork() {
	__asm
	{
		call dword ptr [eax + 0xC]

		mov edx, dword ptr [eax]
		mov ecx, eax

		call dword ptr [edx + 0xC]

		mov ecx, dword ptr [eax + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		lea ecx, dword ptr [edx + eax + 0x4]
		mov eax, dword ptr [ecx]

		push 0x13
		call dword ptr [eax + 0x294]
		test al, al
		jz TEMPTRESS_END
		jmp[Temptress_return]

	TEMPTRESS_END:
		jmp[Temptress_end]
	}
}
