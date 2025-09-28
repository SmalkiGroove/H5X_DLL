#include "pch.h"

// Exhauster also reduce speed by 1

void ExhausterFork();

int Exhauster_fork = 0x008A2063;
int Exhauster_return = 0x008A2074;
int Exhauster_end = 0x008A207F;

void ExhausterTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Exhauster_fork, 13, ExhausterFork, 0, 0, 0 });
}

__declspec(naked) void ExhausterFork() {
	__asm
	{
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
		test eax, eax
		je EXHAUSTER_END

		mov ecx, dword ptr [eax + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		lea ecx, dword ptr [edx + eax + 0x4]
		mov eax, dword ptr [ecx]
		
		push 0x2F
		call dword ptr [eax + 0x294]
		test al, al
		jz EXHAUSTER_END
		jmp[Exhauster_return]

	EXHAUSTER_END:
		jmp[Exhauster_end]
	}
}
