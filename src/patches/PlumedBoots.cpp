#include "pch.h"
#include "types/Artifacts.h"
#include "structs/Creature.h"

// ARTIFACT_PLUMED_BOOTS (49): army creatures fly in combat when owner hero wears the boots.
// Hooks CombatCreature vtable +0x190 (func_CombatCreature_GetSpeedPenalty @ 0x008A22D0).

void PlumedBootsFlyingFork();

int PlumedBootsFlying_fork = 0x008A22D0;
int PlumedBootsFlying_vanilla = 0x004BA950;

void PlumedBoots_init(pugi::xml_document& doc) {
    // mov ecx,[ecx-0xA0] (6) + jmp func_GetCreatureFlying (5) = 11 bytes
    assembly_patches.push_back({ PATCH_HOOK, PlumedBootsFlying_fork, 11, PlumedBootsFlyingFork, 0, 0, 0 });
}

static bool __fastcall owner_has_plumed_boots(ICombatCreature* creature) {
    ICombatHero* owner = creature->owner();
    if (!owner) return false;
    return owner->has_artifact(ARTIFACT_PLUMED_BOOTS);
}

// Hook @ func_CombatCreature_GetSpeedPenalty (0x008A22D0).
// Entry: ECX = combat stack root (after vtordisp @ 0x008AEB60); AL = Flying flag on return.
__declspec(naked) void PlumedBootsFlyingFork() {
    __asm
    {
        push ecx
        call owner_has_plumed_boots
        test al, al
        jnz PLUMED_FLYING_TRUE

        pop ecx
        mov ecx, dword ptr [ecx - 0xA0]
        jmp[PlumedBootsFlying_vanilla]

        PLUMED_FLYING_TRUE:
        add esp, 4
        mov al, 1
        ret
    }
}
