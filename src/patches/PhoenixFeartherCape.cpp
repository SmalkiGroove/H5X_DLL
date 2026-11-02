#include "pch.h"

// PHOENIX FEATHER CAPE (85) IMMUNES TO FROST POWER

void PhoenixFeatherCapeFork();

int PhoenixFeatherCape_fork = 0x004E66C2;
int PhoenixFeatherCape_return = 0x004E66C7;
int PhoenixFeatherCape_end = 0x004E6730;

void PhoenixFeatherCape_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, PhoenixFeatherCape_fork, 5, PhoenixFeatherCapeFork, 0, 0, 0 });
}

// target = unit hit by Frost Power: immune when its own hero wears the cape
static bool __fastcall phoenix_feather_cape_protects(ICombatUnit* target) {
    IHero* hero = owner_hero(target);
    return hero && hero->has_artifact(ARTIFACT_PHOENIX_FEATHER_CAPE);
}

// edi = target unit
__declspec(naked) void PhoenixFeatherCapeFork() {
    __asm
    {
        mov ecx, edi
        call phoenix_feather_cape_protects
        test al, al
        jne PHOENIX_FEATHER_CAPE_END

        call [get_defaultstats]
        jmp [PhoenixFeatherCape_return]

    PHOENIX_FEATHER_CAPE_END:
        jmp [PhoenixFeatherCape_end]
    }
}
