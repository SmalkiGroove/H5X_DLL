#include "pch.h"
#include "types/Artifacts.h"

// EDIT ENEMY CREATURES DEFENSE FROM ARTIFACTS
// ARTIFACT_SHACKLES_OF_THE_LAST_MAN (159) -2 defense
// ARTIFACT_OGRE_CLUB (196) -25% defense

void CreatureDefenseFork();

int CreatureDefense_fork = 0x008A25ED;
int CreatureDefense_return = 0x008A25F4;

void CreatureDefense_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureDefense_fork, 7, CreatureDefenseFork, 0, 0, 0 });
}

// Percent first, then flat (same order as ModAttack).
static int __fastcall apply_enemy_defense_artifacts(int* inventory, int defense) {
    if (!inventory || defense <= 0) return defense;
    if (count_equipped_artifact(inventory, ARTIFACT_OGRE_CLUB) != 0)
        defense = (int)(defense * 0.75f);
    if (count_equipped_artifact(inventory, ARTIFACT_SHACKLES_OF_THE_LAST_MAN) != 0)
        defense -= 2;
    return defense;
}

// Hook at 0x008A25ED, end of func_GetUnitDefense core (impl 0x008A22F0, called from vtable +0x19C @ 0x008A32B0).
// Overwritten game code: xor eax,eax / test ebx,ebx / setle al.
// At entry: esi = combat-unit outer (vbtable at [esi+4]), ebx = final defense before clamp.
__declspec(naked) void CreatureDefenseFork() {
    __asm
    {
        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + esi + 0x4]
        lea ecx, dword ptr [edx + esi + 0x4]
        call dword ptr [eax + 0x10]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xc]
        test eax, eax
        je CREATURE_DEFENSE_CLAMP
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        mov edx, ebx
        call apply_enemy_defense_artifacts
        mov ebx, eax

    CREATURE_DEFENSE_CLAMP:
        xor eax, eax
        test ebx, ebx
        setle al
        jmp[CreatureDefense_return]
    }
}
