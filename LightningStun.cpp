#include "pch.h"

// LIGHTNING SPELLS ARE ALWAYS AFFECTED BY MASTER OF LIGHTNING PERK

void LightningStunFork();

int LightningStun_fork = 0x004E5C36;
int LightningStun_return = 0x004E5C4A;

void LightningStun_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, LightningStun_fork, 14, LightningStunFork, 0, 0, 0 });
}

__declspec(naked) void LightningStunFork() {
    __asm
    {
        jmp[LightningStun_return]
    }
}
