#include "pch.h"

// EDIT DEFENSE FROM ARTIFACTS
// 

void CreatureDefenseFork();

int CreatureDefense_fork = 0x00989E4E;
int CreatureDefense_return = 0x00989E70;

void CreatureDefense_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureDefense_fork, 5, CreatureDefenseFork, 0, 0, 0 });
}

__declspec(naked) void CreatureDefenseFork() {
    __asm
    {

    }
}
