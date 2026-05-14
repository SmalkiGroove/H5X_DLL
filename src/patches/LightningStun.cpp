#include "pch.h"

// LIGHTNING SPELLS ARE ALWAYS AFFECTED BY MASTER OF LIGHTNING PERK

void LightningStunFork1();
void LightningStunFork2();

int LightningStun_fork1 = 0x004E5C36;
int LightningStun_return1 = 0x004E5C4A;
int LightningStun_fork2 = 0x009BB8EC;
int LightningStun_return2 = 0x009BB8F8;

void LightningStun_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, LightningStun_fork1, 14, LightningStunFork1, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LightningStun_fork2, 12, LightningStunFork2, 0, 0, 0 });
}

__declspec(naked) void LightningStunFork1() {
    __asm
    {
        jmp[LightningStun_return1]
    }
}

__declspec(naked) void LightningStunFork2() {
    __asm
    {
        mov eax, 0x1
        jmp[LightningStun_return2]
    }
}
