#include "pch.h"

// EDIT ATTACK FROM ARTIFACTS
// 

void CreatureAttackFork();

int CreatureAttack_fork = 0x004BB279;
int CreatureAttack_return = 0x004BB27F;

void CreatureAttack_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureAttack_fork, 6, CreatureAttackFork, 0, 0, 0 });
}

__declspec(naked) void CreatureAttackFork() {
    __asm
    {
        test esi, esi
        jz NO_ENEMY_HERO_SKIP

        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]

    NO_ENEMY_HERO_SKIP:
        mov eax, dword ptr [esi]
        mov ecx, esi
        call dword ptr [eax + 0x330]
    }
}
