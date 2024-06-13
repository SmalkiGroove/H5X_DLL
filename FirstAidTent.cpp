#include "pch.h"

// EDIT FIRST AID TENT DATA

void FirstAidTentFork();

int FirstAidHeal_fork = 0x009771F0;
int FirstAidHeal_return = 0x009771F6;

void FirstAidTent_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x009771A6, 4, nullptr, 20, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771B4, 4, nullptr, 40, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C2, 4, nullptr, 60, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D0, 4, nullptr, 80, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771C8, 4, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x009771D6, 4, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00E0915A, 4, nullptr, 1, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00E0915E, 4, nullptr, 2, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00E09162, 4, nullptr, 3, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00E09166, 4, nullptr, 4, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, 0x009771F0, 6, FirstAidTentFork, 0, 0, 0 });
}

__declspec(naked) void FirstAidTentFork() {
	__asm
	{
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		mov edx, dword ptr [ecx + esi + 0x4]
		lea ecx, dword ptr [ecx + esi + 0x4]
		call dword ptr [edx + 0x19C]
		push eax
		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax]
		mov edx, dword ptr [eax]
		push 0x2
		mov ecx, eax
		call dword ptr [edx + 0x174]
		pop ecx
		mov eax, dword ptr [eax*4 + 0xE0915A]
		mul ecx
		add dword ptr [edi], eax
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax]
		mov edx, dword ptr [eax]
		push 0xB
		mov ecx, eax
		call dword ptr [edx + 0x174]
		mov dword ptr [esp + 0x3C], eax
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		jmp[FirstAidHeal_return]
	}
}
