#include "pch.h"

// EDIT HARDCODED HERO SPECIALIZATIONS


// Coven Mistress also applies on non-damaging and mass spells

void CovenMistressTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00978E30, 18, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00978F30, 3, nullptr, 0, 0, 0, "681E90" });
}


// Master of Element no longer summon Elementals on friendly creature death

void MasterOfElementsTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x00A5A60E, 6, nullptr, 0, 0, 0, "E91D01000090" });
}

// Embalmer formula reworked

void EmbalmerFork();

int Embalmer_fork = 0x0097721E;
int Embalmer_return = 0x00977223;

void EmbalmerTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Embalmer_fork, 5, EmbalmerFork, 0, 0, 0 });
}

__declspec(naked) void EmbalmerFork() {
	__asm
	{
		push eax
		mov ecx, dword ptr ds : [esi]
		push ecx
		fstp dword ptr ds : [esi]
		mov ecx, dword ptr ds : [esi]
		push ecx
		mov dword ptr ds : [esi], eax
		//mov ecx, 0x5
		mul ecx
		add dword ptr ds : [edi], eax
		fld dword ptr ds : 0x00400F18 // 0
		fimul dword ptr ds : [esi]
		fimul dword ptr ds : [edi]
		fistp dword ptr ds : [esi]
		mov ecx, dword ptr ds : [esi]
		add dword ptr ds : [edi] , ecx
		pop ecx
		mov dword ptr ds : [esi] , ecx
		fld dword ptr ds : [esi]
		pop ecx
		mov dword ptr ds : [esi] , ecx
		pop eax
		jmp[Embalmer_return]
	}
}
