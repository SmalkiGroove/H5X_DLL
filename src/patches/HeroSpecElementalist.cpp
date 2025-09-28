#include "pch.h"

// Elementalist doubles elemental chains damages

void ElementalistFork();

int Elementalist_fork = 0x004B0E43;
int Elementalist_return = 0x004B0E4D;

void ElementalistChain_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Elementalist_fork, 10, ElementalistFork, 0, 0, 0 });
}

__declspec(naked) void ElementalistFork() {
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
		push 0x92
		call dword ptr [eax + 0x290]
		test al, al
		jz ELEMENTALIST_RETURN
		fld dword ptr [esp]
		fadd st(0), st(0)
		fstp dword ptr [esp]

	ELEMENTALIST_RETURN:
		mov ecx, dword ptr [esi + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		mov eax, dword ptr [edx + esi + 0x4]
		jmp[Elementalist_return]
	}
}
