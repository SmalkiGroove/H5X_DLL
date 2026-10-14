#include "pch.h"
#include "types/Artifacts.h"
#include "Artfset.h"

// EDIT ENEMY CREATURES ATTACK FROM ARTIFACTS
// ARTIFACT_SHACKLES_OF_THE_LAST_MAN (159) -2 attack
// ARTIFACT_OGRE_SHIELD (197) -25% attack
// Spirit set (26), 5 pieces : -20% attack (ArtfsetSpirit.cpp)

void CreatureAttackFork();

int CreatureAttack_fork = 0x008A4B5C;    // end of func_GetUnitAttack (CombatUnit +0x198, impl 0x008A4800): xor eax,eax / test ebx,ebx / setle al
int CreatureAttack_return = 0x008A4B63;

void CreatureAttack_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureAttack_fork, 7, CreatureAttackFork, 0, 0, 0 });
}

// Attack of a creature lowered by the enemy hero's artifacts. Percent first, then flat.
static int __fastcall enemy_attack_artifacts(ICombatCreature* unit, int attack) {
    if (attack <= 0) return attack;
    ICombatHero* enemy = unit->opponent();
    IHero* hero = enemy ? enemy->hero() : nullptr;
    if (!hero) return attack;
    if (hero->has_artifact(ARTIFACT_OGRE_SHIELD))
        attack = (int)(attack * 0.75f);
    attack = (int)(attack * artfset_spirit_enemy_stat_multiplier(hero));
    if (hero->has_artifact(ARTIFACT_SHACKLES_OF_THE_LAST_MAN))
        attack -= 2;
    return attack;
}

// esi = creature CombatUnit subobject, ebx = final attack before the >= 0 clamp
__declspec(naked) void CreatureAttackFork() {
    __asm
    {
        mov ecx, esi
        mov edx, ebx
        call enemy_attack_artifacts
        mov ebx, eax

        xor eax, eax
        test ebx, ebx
        setle al
        jmp [CreatureAttack_return]
    }
}
