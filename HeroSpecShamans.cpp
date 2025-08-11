#include "pch.h"

// Shaman spec double casters mana and spellpower

void ShamanMatriarchManaFork();
void ShamanMatriarchSpwrFork();

int ShamanMatriarchMana_fork = 0x004BAB46;
int ShamanMatriarchMana_return = 0x004BAB53;
int ShamanMatriarchMana_end = 0x004BAB65;

int ShamanMatriarchSpwr_fork = 0x008A3B01;
int ShamanMatriarchSpwr_return = 0x008A3B12;
int ShamanMatriarchSpwr_end = 0x008A3B35;

void ShamanMatriarchTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShamanMatriarchMana_fork, 13, ShamanMatriarchManaFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ShamanMatriarchSpwr_fork, 17, ShamanMatriarchSpwrFork, 0, 0, 0 });
}

__declspec(naked) void ShamanMatriarchManaFork() {
	__asm
	{
		push 0x40
		call dword ptr [eax + 0x190]
		test al, al
		jz SHAMAN_MATRIARCH_MANA_END
		jmp[ShamanMatriarchMana_return]

	SHAMAN_MATRIARCH_MANA_END:
		jmp[ShamanMatriarchMana_end]
	}
}

__declspec(naked) void ShamanMatriarchSpwrFork() {
	__asm
	{
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]

		mov ecx, dword ptr [eax + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		lea ecx, dword ptr [edx + eax + 0x4]
		mov eax, dword ptr [ecx]

		push 0x40
		call dword ptr [eax + 0x294]
		test al, al
		jz SHAMAN_MATRIARCH_SPWR_END
		jmp[ShamanMatriarchSpwr_return]

	SHAMAN_MATRIARCH_SPWR_END:
		jmp[ShamanMatriarchSpwr_end]
	}
}
