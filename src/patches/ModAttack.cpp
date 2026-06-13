#include "pch.h"
#include "types/Artifacts.h"

// EDIT ENEMY CREATURES ATTACK FROM ARTIFACTS
// ARTIFACT_SHACKLES_OF_THE_LAST_MAN (159) -2 attack
// ARTIFACT_OGRE_SHIELD (197) -25% attack

void CreatureAttackFork();

int CreatureAttack_fork = 0x008A4B5C;
int CreatureAttack_return = 0x008A4B63;

void CreatureAttack_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureAttack_fork, 7, CreatureAttackFork, 0, 0, 0 });
}

// Percent first, then flat (user preference).
static int __fastcall apply_enemy_attack_artifacts(int* inventory, int attack) {
    if (!inventory || attack <= 0) return attack;
    if (count_equipped_artifact(inventory, ARTIFACT_OGRE_SHIELD) != 0)
        attack = (int)(attack * 0.75f);
    if (count_equipped_artifact(inventory, ARTIFACT_SHACKLES_OF_THE_LAST_MAN) != 0)
        attack -= 2;
    return attack;
}

// Hook at 0x008A4B5C, end of func_GetUnitAttack (vtable +0x198, impl 0x008A4800).
// Overwritten game code: xor eax,eax / test ebx,ebx / setle al.
// At entry: esi = ICombatCreature*, ebx = final attack before clamp.
__declspec(naked) void CreatureAttackFork() {
    __asm
    {
        mov ecx, dword ptr [esi - 0x144]
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + esi - 0x144]
        lea ecx, dword ptr [edx + esi - 0x144]
        call dword ptr [eax + 0x10]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xc]
        test eax, eax
        je CREATURE_ATTACK_CLAMP
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        mov edx, ebx
        call apply_enemy_attack_artifacts
        mov ebx, eax

    CREATURE_ATTACK_CLAMP:
        xor eax, eax
        test ebx, ebx
        setle al
        jmp[CreatureAttack_return]
    }
}
