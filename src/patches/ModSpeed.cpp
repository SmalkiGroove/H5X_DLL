#include "pch.h"
#include "Artfset.h"

// ADD SPEED FROM ADDITIONAL ARTIFACTS
// (157) Harpy Boots : +1 Speed
// Orcs set (18), 6 pieces : +2 Speed (ArtfsetOrcs.cpp)
// Bandit set (28), 5 pieces : +1 Speed (ArtfsetBandit.cpp)

void SpeedFork();

int Speed_fork = 0x004BC139;    // creature speed: vanilla artifact bonus (Boots of Swiftness +1, Ring of Caution -1) up to sub ebx,eax
int Speed_return = 0x004BC161;  // call [edx+330h], then ebx is added to the speed

void Speed_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Speed_fork, 40, SpeedFork, 0, 0, 0 });
}

// Speed added by the hero's artifacts and artifact sets (replaces the vanilla value kept in ebx).
static int __fastcall speed_artifact_bonus(IHero* hero) {
    int* inventory = hero->inventory();
    return count_equipped_artifact(inventory, ARTIFACT_HARPY_BOOTS)
        + artfset_orcs_speed(hero)
        + artfset_bandit_speed(hero);
}

// esi = IHero, edi = speed accumulator (kept). At return the game expects ebx = artifact bonus,
// edx = [esi] and ecx = esi for its call [edx+330h].
__declspec(naked) void SpeedFork() {
    __asm
    {
        mov ecx, esi
        call speed_artifact_bonus
        mov ebx, eax
        mov edx, dword ptr [esi]
        mov ecx, esi
        jmp [Speed_return]
    }
}
