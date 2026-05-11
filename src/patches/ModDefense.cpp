#include "pch.h"

// EDIT DEFENSE FROM ARTIFACTS
// 

void CreatureDefenseFork();

int CreatureDefense_fork = 0x004BB1D2;
int CreatureDefense_return = 0x004BB1D7;

void CreatureDefense_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureDefense_fork, 5, CreatureDefenseFork, 0, 0, 0 });
}

__declspec(naked) void CreatureDefenseFork() {
    __asm
    {


        mov edx, dword ptr [esi]
        mov ecx, esi
        push ebp
    }
}
