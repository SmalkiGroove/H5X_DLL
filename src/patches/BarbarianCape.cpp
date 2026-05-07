#include "pch.h"

// DISABLE COWARDICE AND IMMUNE TO FEAR

void BarbarianCapeFork();

int BarbarianCape_fork = 0x00A62D2A;
int BarbarianCape_return = 0x00A62D2F;
int BarbarianCape_end = 0x00A62D70;

void BarbarianCape_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, BarbarianCape_fork, 5, BarbarianCapeFork, 0, 0, 0 });
}

__declspec(naked) void BarbarianCapeFork() {
    __asm
    {
        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ecx]
        call dword ptr [edx + 0xC]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
		test eax, eax
		je SKIP_BARBARIAN_CAPE
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0x74]
		mov ecx, eax
		push 0x92
		call[count_equipped_artifact]
		test eax, eax
		jne ACTIVATE_BARBARIAN_CAPE

        SKIP_BARBARIAN_CAPE:
        call[get_defaultstats]
        jmp[BarbarianCape_return]

		ACTIVATE_BARBARIAN_CAPE:
		jmp[BarbarianCape_end]
    }
}
