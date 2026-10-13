#include "pch.h"
#include "types/HeroSpecs.h"
#include "Artfset.h"

// ADD MORALE BUFFS FROM ADDITIONAL SOURCES
// skill Motivation +1 Morale
// skill Last Stand +1 Morale
// skill Despotism -1 Morale
// Orcs set (18), 6 pieces : +2 Morale (ArtfsetOrcs.cpp)

// ADD MORALE DEBUFFS FROM ADDITIONAL SOURCES
// (175) Spirit of Oppression : -1 Morale
// (176) Crimson Scythe : -1 Morale
// (125) Rod of Torment : -2 Morale
// skill Intimidating Voice -1 Morale
// skill Dead Man's Curse -3 Morale
// spec 80 (Sovereign) -1 Morale

void MoraleBuffFork();
void MoraleDebuffFork();

int MoraleBuff_fork = 0x00B4CAFB;      // IHero get_morale_raw (0x00B4CAE0), before the Pariah check: mov edx,[esi] / push 53h / mov ecx,esi
int MoraleBuff_return = 0x00B4CB01;
int MoraleDebuff_fork = 0x0098A175;    // func_ComputeMoraleModifiers, enemy hero debuffs, last vanilla one: neg eax / lea esi,[esi+eax*2]
int MoraleDebuff_return = 0x0098A17A;

void Morale_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, MoraleBuff_fork, 6, MoraleBuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, MoraleDebuff_fork, 5, MoraleDebuffFork, 0, 0, 0 });
}

// Morale from the hero's own skills and artifact sets, added to the hero's base morale.
static int __fastcall hero_morale_bonus(IHero* hero) {
    int bonus = 0;
    if (hero->skill_mastery(PERK_MOTIVATION) > 0)  bonus += 1;
    if (hero->skill_mastery(PERK_LAST_STAND) > 0)  bonus += 1;
    if (hero->skill_mastery(SKILL_DESPOTISM) > 0)  bonus -= 1;
    bonus += artfset_orcs_morale(hero);
    return bonus;
}

// Morale removed from a unit by the enemy hero (NWorld::ICombatHero interface, not null).
static int __fastcall enemy_morale_malus(int* enemy_hero_interface) {
    ICombatHero* enemy = combat_creature_detail::combat_hero_from_outer(enemy_hero_interface);
    int malus = 0;
    IHero* hero = enemy->hero();
    if (hero) {
        int* inventory = hero->inventory();
        malus += count_equipped_artifact(inventory, ARTIFACT_SPIRIT_OF_OPPRESSION);
        malus += 2 * count_equipped_artifact(inventory, ARTIFACT_ROD_OF_TORMENT);
        malus += count_equipped_artifact(inventory, ARTIFACT_CRIMSON_SCYTHE);
    }
    if (enemy->has_perk(PERK_INTIMIDATING_VOICE))   malus += 1;
    if (enemy->has_perk(ULTI_DEAD_MANS_CURSE))      malus += 3;
    if (enemy->has_spec(HERO_SPEC_DEMON_HUNTER))    malus += 1;  // Sovereign
    return malus;
}

// esi = IHero, edi = morale being summed
__declspec(naked) void MoraleBuffFork() {
    __asm
    {
        mov ecx, esi
        call hero_morale_bonus
        add edi, eax

        mov edx, dword ptr [esi]
        push 0x53
        mov ecx, esi
        jmp [MoraleBuff_return]
    }
}

// edi = enemy hero (NWorld::ICombatHero interface), esi = morale being summed, eax = vanilla count to replay
__declspec(naked) void MoraleDebuffFork() {
    __asm
    {
        neg eax
        lea esi, dword ptr [esi + eax * 2]

        mov ecx, edi
        call enemy_morale_malus
        sub esi, eax
        jmp [MoraleDebuff_return]
    }
}
