#include "pch.h"

// FIRE SPELLS ARE ALWAYS AFFECTED BY MASTER OF FIRE PERK

void FireBurnFork1();
void FireBurnFork2();

int FireBurn_fork1 = 0x004E5B8D;
int FireBurn_return1 = 0x004E5BA1;
int FireBurn_fork2 = 0x009BB91E;
int FireBurn_return2 = 0x009BB92A;

void FireBurn_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, FireBurn_fork1, 20, FireBurnFork1, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, FireBurn_fork2, 12, FireBurnFork2, 0, 0, 0 });
}

__declspec(naked) void FireBurnFork1() {
    __asm
    {
        jmp[FireBurn_return1]
    }
}

__declspec(naked) void FireBurnFork2() {
    __asm
    {
        mov eax, 0x1
        jmp[FireBurn_return2]
    }
}
