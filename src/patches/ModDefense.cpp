#include "pch.h"
#include "types/Artifacts.h"

// EDIT ENEMY CREATURES DEFENSE FROM ARTIFACTS
// ARTIFACT_SHACKLES_OF_THE_LAST_MAN (159) -2 defense
// ARTIFACT_OGRE_CLUB (196) -25% defense

void CreatureDefenseFork();

int CreatureDefense_fork = 0x008A25ED;    // end of func_GetUnitDefense (impl 0x008A22F0, from CombatUnit +0x19C): xor eax,eax / test ebx,ebx / setle al
int CreatureDefense_return = 0x008A25F4;  // MasterOfFireFix.cpp hooks there

void CreatureDefense_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureDefense_fork, 7, CreatureDefenseFork, 0, 0, 0 });
}

// Defense of a creature lowered by the enemy hero's artifacts. Percent first, then flat (same order as ModAttack).
static int __fastcall enemy_defense_artifacts(ICombatCreature* unit, int defense) {
    if (defense <= 0) return defense;
    ICombatHero* enemy = unit->opponent();
    IHero* hero = enemy ? enemy->hero() : nullptr;
    if (!hero) return defense;
    if (hero->has_artifact(ARTIFACT_OGRE_CLUB))
        defense = (int)(defense * 0.75f);
    if (hero->has_artifact(ARTIFACT_SHACKLES_OF_THE_LAST_MAN))
        defense -= 2;
    return defense;
}

// esi = creature complete object (vbptr at esi+4), ebx = final defense before the >= 0 clamp
__declspec(naked) void CreatureDefenseFork() {
    __asm
    {
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        lea ecx, dword ptr [ecx + esi + 0x4]    // CombatUnit subobject
        mov edx, ebx
        call enemy_defense_artifacts
        mov ebx, eax

        xor eax, eax
        test ebx, ebx
        setle al
        jmp [CreatureDefense_return]
    }
}
