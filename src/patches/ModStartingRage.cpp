#include "pch.h"
#include "Artfset.h"

// RAGE POINTS GAINED AT COMBAT START (rage users only: Stronghold creatures)
// (152) Collar of Primal Rage : +100
// Orcs set (18), 4 / 6 pieces + Stronghold hero : +100 / +250 (ArtfsetOrcs.cpp)

void StartingRageFork();

int StartingRage_fork = 0x008AB32D;    // creature combat start (esi = CCombatCreature, ebp = its combat hero, not null): mov ecx,[ebp+4] / mov edx,[ecx+8]
int StartingRage_return = 0x008AB333;  // hero skill 0xB0 / 0xAE rage checks

static const int collar_starting_rage = 100;

void StartingRage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, StartingRage_fork, 6, StartingRageFork, 0, 0, 0 });
}

// hero_interface = NWorld::ICombatHero (vtable slot 0 = IHero), creature = complete object (ICombatStack at +0xA0)
static void __fastcall starting_rage(int* hero_interface, char* creature) {
    IHero* hero = (IHero*)((GetPtr)(*(int***)hero_interface)[0])(hero_interface);
    if (!hero) return;
    int rage = artfset_orcs_starting_rage(hero);
    if (hero->has_artifact(ARTIFACT_COLLAR_OF_PRIMAL_RAGE)) rage += collar_starting_rage;
    if (rage > 0)
        ((ICombatStack*)(creature + 0xA0))->add_rage(rage, 0x2710, 1);  // same time / flag as the vanilla skill bonuses below
}

__declspec(naked) void StartingRageFork() {
    __asm
    {
        mov edx, esi
        mov ecx, ebp
        call starting_rage

        mov ecx, dword ptr [ebp + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        jmp [StartingRage_return]
    }
}
