#include "pch.h"

// (177) Pirate Smasher : the hero's creatures lower the defense of the creatures they hit by 3, like the Armor Crushing
// mini-artifact effect (Artificier.cpp). Both add up into the same per-hit debuff.

void PirateSmasherFork();

int PirateSmasher_fork = 0x00A62CE4;    // FUN_00a629b0 (after a hit), Armor Crushing value: movsx eax,byte ptr [esp+14h] / test eax,eax
int PirateSmasher_return = 0x00A62CEB;  // jle (no crushing); else target CombatUnit +0x260 (buff 0xB3, permanent, value = eax)

const int pirate_smasher_crushing = 3;

void PirateSmasher_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, PirateSmasher_fork, 7, PirateSmasherFork, 0, 0, 0 });
}

// attacker = creature dealing the hit (heroes never reach this site)
static int __fastcall pirate_smasher_bonus(ICombatUnit* attacker) {
    IHero* hero = owner_hero(attacker);
    return hero && hero->has_artifact(ARTIFACT_PIRATE_SMASHER) ? pirate_smasher_crushing : 0;
}

// esi = attack context (+0x00 attacker), [esp+0x14] = attacker's Armor Crushing (mini-artifact stats copied at [esp+0xC])
__declspec(naked) void PirateSmasherFork() {
    __asm
    {
        mov ecx, dword ptr [esi]
        call pirate_smasher_bonus
        movsx ecx, byte ptr [esp + 0x14]
        add eax, ecx
        test eax, eax
        jmp [PirateSmasher_return]
    }
}
