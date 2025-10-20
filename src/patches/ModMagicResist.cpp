#include "pch.h"

// EDIT MAGIC RESISTANCE BONUSES
// (27) Boots of Interference : 15% MR
// (174) Ancient Protection Charm : 10% MR
// Skill Disrupt Magic : 30% MR

void MagicResistFork();

int MagicResist_fork = 0x0097CFFD;
int MagicResist_return = 0x0097D004;

void MagicResist_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, MagicResist_fork, 7, MagicResistFork, 0, 0, 0 });
    
	// Ancient Protection Charm bonus
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D005, 1, nullptr, 174, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D016, 1, nullptr, 174, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D022, 1, nullptr, 10, 0, 0, 0 });
	// Boots of Interference bonus
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D04C, 1, nullptr, 15, 0, 0, 0 });
    // Disrupt Magic bonus
    assembly_patches.push_back({ PATCH_BYTE, 0x0097CFBE, 1, nullptr, 30, 0, 0, 0 });
}

__declspec(naked) void MagicResistFork() {
    __asm
    {

        MAGIC_RESIST_NEXT_LAST:
        mov edx, dword ptr [esi]
        mov ecx, esi
        call dword ptr [edx+0x74]
        jmp[MagicResist_return]
    }
}
