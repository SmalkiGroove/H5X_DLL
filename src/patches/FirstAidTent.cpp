#include "pch.h"

// EDIT FIRST AID TENT HEAL FORMULA
// Base heal (by First Aid mastery 0..3) = 10/20/30/40, dispel tier 0/1/2/3
// + hero defense x 3/4/5/6 (by the hero's Warfare mastery 0..3)
// + hero level
// + 100 with (109) Ring of Health
// Then vanilla: spec Empiric (36) +5 per level (HeroSpecEmbalmer.cpp), hero +0x314 > 0: heal x2.
// The tent can always resurrect.

void FirstAidTentFork();

int FirstAidHeal_fork = 0x009771F0;      // heal computation (0x00977190), hero known: mov eax,[esi+4] ... lea ecx,[ecx+esi+4]
int FirstAidHeal_return = 0x009771FE;    // push 24h (spec Empiric check)

static const int base_heal[4] = { 10, 20, 30, 40 };
static const int dispel_tier[4] = { 0, 1, 2, 3 };
static const int heal_per_defense[4] = { 3, 4, 5, 6 };

// Immediates of the vanilla switch on the tent's First Aid mastery: mov [edi],heal / mov [edx],dispel tier
static const int base_heal_operand[4] = { 0x009771A6, 0x009771B4, 0x009771C2, 0x009771D0 };
static const int dispel_tier_operand[4] = { 0x009771AC, 0x009771BA, 0x009771C8, 0x009771D6 };

void FirstAidTent_init(pugi::xml_document& doc) {
    for (int mastery = 0; mastery < 4; mastery++) {
        assembly_patches.push_back({ PATCH_INT, base_heal_operand[mastery], 4, nullptr, base_heal[mastery], 0, 0, 0 });
        assembly_patches.push_back({ PATCH_INT, dispel_tier_operand[mastery], 4, nullptr, dispel_tier[mastery], 0, 0, 0 });
    }
    assembly_patches.push_back({ PATCH_HOOK, FirstAidHeal_fork, 14, FirstAidTentFork, 0, 0, 0 });
    // func_CheckUnitEffect: resurrection flag only with the hero's skill 0x16 (jne -> jmp: always on)
    assembly_patches.push_back({ PATCH_WRTE, 0x00986DA0, 1, nullptr, 0, 0, 0, "EB" });
}

// hero_interface = the tent owner's NWorld::ICombatHero (not null), heal = the heal being computed
static void __fastcall first_aid_heal_bonus(int* hero_interface, int* heal) {
    ICombatHero* hero = combat_creature_detail::combat_hero_from_outer(hero_interface);
    IHero* adventure_hero = hero->hero();

    int warfare = adventure_hero ? adventure_hero->skill_mastery(SKILL_WARFARE) : 0;
    if (warfare < 0) warfare = 0;
    if (warfare > 3) warfare = 3;
    *heal += hero->defense() * heal_per_defense[warfare];
    *heal += hero->hero_level();
    if (adventure_hero && adventure_hero->has_artifact(ARTIFACT_RING_OF_HEALTH))
        *heal += 100;
}

// esi = tent owner (NWorld::ICombatHero interface, vbptr at esi+4), edi = &heal
__declspec(naked) void FirstAidTentFork() {
    __asm
    {
        mov ecx, esi
        mov edx, edi
        call first_aid_heal_bonus

        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]
        jmp [FirstAidHeal_return]
    }
}
