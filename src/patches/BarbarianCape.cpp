#include "pch.h"

// DISABLE COWARDICE AND IMMUNE TO FEAR

void BarbarianCapeFork();

int BarbarianCape_fork = 0x00A62D2A;
int BarbarianCape_return = 0x00A62D2F;
int BarbarianCape_end = 0x00A62D70;

void BarbarianCape_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, BarbarianCape_fork, 5, BarbarianCapeFork, 0, 0, 0 });
}

// target = unit getting the fear effect: protected when its own hero wears the cape
static bool __fastcall barbarian_cape_protects(ICombatUnit* target) {
    IHero* hero = owner_hero(target);
    return hero && hero->has_artifact(ARTIFACT_BARBARIAN_CAPE);
}

// [esi+4] = target unit
__declspec(naked) void BarbarianCapeFork() {
    __asm
    {
        mov ecx, dword ptr [esi + 0x4]
        call barbarian_cape_protects
        test al, al
        jne ACTIVATE_BARBARIAN_CAPE

        call [get_defaultstats]
        jmp [BarbarianCape_return]

    ACTIVATE_BARBARIAN_CAPE:
        jmp [BarbarianCape_end]
    }
}
