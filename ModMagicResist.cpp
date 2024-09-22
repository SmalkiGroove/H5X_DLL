#include "pch.h"

// EDIT MAGIC RESISTANCE FROM ARTIFACTS
// (27) Boots of Interference : 15% MR
// (174) Ancient Protection Charm : 10% MR

void MagicResistFork();

int MagicResist_fork = 0x0097CFFD;
int MagicResist_return = 0x0097D004;

void MagicResist_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, MagicResist_fork, 7, MagicResistFork, 0, 0, 0 });

    assembly_patches.push_back({ PATCH_BYTE, 0x0097D005, 1, nullptr, 174, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D016, 1, nullptr, 174, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D022, 1, nullptr, 10, 0, 0, 0 });

    //assembly_patches.push_back({ PATCH_BYTE, 0x0097D02F, 1, nullptr, 27, 0, 0, 0 });
    //assembly_patches.push_back({ PATCH_BYTE, 0x0097D040, 1, nullptr, 27, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_BYTE, 0x0097D04C, 1, nullptr, 15, 0, 0, 0 });
}

__declspec(naked) void MagicResistFork() {
    __asm
    {

    }
}
