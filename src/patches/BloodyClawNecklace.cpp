#include "pch.h"

// INCREASE CREATRUES MAX DAMAGES BY 25%

void BloodyClawNecklaceFork();

int BloodyClawNecklace_fork = 0x004BC05C;
int BloodyClawNecklace_return = 0x004BC062;

void BloodyClawNecklace_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, BloodyClawNecklace_fork, 6, BloodyClawNecklaceFork, 0, 0, 0 });
}

static int __fastcall bloody_claw_max_damage(IHero* hero, int max_damage) {
    if (!hero->has_artifact(ARTIFACT_NECKLACE_OF_THE_BLOODY_CLAW)) return max_damage;
    return max_damage + (int)((unsigned int)max_damage >> 2);
}

// esi = IHero, edi = creature max damage
__declspec(naked) void BloodyClawNecklaceFork() {
    __asm
    {
        mov edx, edi
        mov ecx, esi
        call bloody_claw_max_damage
        mov edi, eax

        mov edx, dword ptr [esi]
        mov ecx, esi
        push 0x24
        jmp [BloodyClawNecklace_return]
    }
}
