#include "pch.h"

// Perception of Weakness increases spell damage by 15%

void PerceptionOfWeaknessFork();

int PerceptionOfWeakness_fork = 0x004B0E43;
int PerceptionOfWeakness_return = 0x004B0E49;

void PerceptionOfWeakness_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PerceptionOfWeakness_fork, 6, PerceptionOfWeaknessFork, 0, 0, 0 });
}

__declspec(naked) void PerceptionOfWeaknessFork() {
	__asm
	{
		mov ecx, dword ptr [esi + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		mov eax, dword ptr [edx + esi + 0x4]
		lea ecx, dword ptr [edx + esi + 0x4]
		call dword ptr [eax + 0x10]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
		mov ecx, dword ptr [eax + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		lea ecx, dword ptr [edx + eax + 0x4]
		mov eax, dword ptr [ecx]
		push 0xD3
		call dword ptr [eax + 0x290]
		test al, al
		je PERCEPTION_OF_WEAKNESS_RETURN
		fld dword ptr [esp]
		fadd dword ptr [constf_0_15]
		fstp dword ptr [esp]

		PERCEPTION_OF_WEAKNESS_RETURN:
		mov ecx, dword ptr[esi + 0x4]
		mov edx, dword ptr[ecx + 0x8]
		jmp[PerceptionOfWeakness_return]
	}
}
