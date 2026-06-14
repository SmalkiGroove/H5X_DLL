#include "pch.h"
#include "types/Artifacts.h"

// ARTIFACT_PLUMED_BOOTS (49): army creatures fly in combat when owner hero wears the boots.
// Hooks CombatCreature vtable +0x190 (func_CombatCreature_GetSpeedPenalty @ 0x008A22D0).

void PlumedBootsFlyingFork();

int PlumedBootsFlying_fork = 0x008A22D0;
int PlumedBootsFlying_vanilla = 0x004BA950;

void PlumedBoots_init(pugi::xml_document& doc) {
    // mov ecx,[ecx-0xA0] (6) + jmp func_GetCreatureFlying (5) = 11 bytes
    assembly_patches.push_back({ PATCH_HOOK, PlumedBootsFlying_fork, 11, PlumedBootsFlyingFork, 0, 0, 0 });
}

// Owner hero inventory via ModDefense chain; ECX = combat-unit outer on entry.
static bool __fastcall owner_has_plumed_boots(int* unit_outer) {
    __asm
    {
        mov esi, ecx
        test esi, esi
        jz PLUMED_NO_BOOTS
        mov ecx, dword ptr [esi + 0x4]
        test ecx, ecx
        jz PLUMED_NO_BOOTS
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + esi + 0x4]
        lea ecx, dword ptr [edx + esi + 0x4]
        call dword ptr [eax + 0x10]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
        test eax, eax
        jz PLUMED_NO_BOOTS
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        test eax, eax
        jz PLUMED_NO_BOOTS
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        test eax, eax
        jz PLUMED_NO_BOOTS
        mov ecx, eax
        push ARTIFACT_PLUMED_BOOTS
        call[count_equipped_artifact]
        test eax, eax
        jz PLUMED_NO_BOOTS
        mov al, 1
        ret

        PLUMED_NO_BOOTS:
        xor al, al
        ret
    }
}

// Hook @ func_CombatCreature_GetSpeedPenalty (0x008A22D0).
// Entry: ECX = ICombatCreature* subobject; AL = Flying flag on return.
__declspec(naked) void PlumedBootsFlyingFork() {
    __asm
    {
        push ecx
        mov ecx, dword ptr [ecx - 0xA0]
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
