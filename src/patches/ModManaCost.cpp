#include "pch.h"
#include "types/Spells.h"

// ADD MANA COST MODIFIERS FROM ADDITIONAL ARTIFACTS
// (47) Ring of the Magister : -4 mana
// (116) Genji's Hat : -1 mana
// (186) Moonlight Coat : -25% mana
// (101) Crown of the Frost Lord : -50% mana for frost spells
// (151) Blazing Spellbook : -33% mana for fire spells
// (140) Sylvan Amulet : -3 mana for nature spells
// (154) Deadwood Staff : -2 mana for dark spells
// Skill Blaze Power (231) : -10% mana for fire spells
// Skill Frost Power (232) : -10% mana for ice spells
// Skill Thunder Power (233) : -10% mana for lightning spells
// Skill Earth Power (234) : -10% mana for earth spells

void ManaCostFork();

int ManaCost_fork = 0x00A348D8;
int ManaCost_return = 0x00A348E7;

void ManaCost_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ManaCost_fork, 15, ManaCostFork, 0, 0, 0 });
}

// cost x 0.9, with the x87 instructions of the original code (same rounding at the game's FPU precision)
static int ninety_percent(int cost) {
    int result;
    __asm {
        fild dword ptr [cost]
        fmul dword ptr [constf_0_9]
        fistp dword ptr [result]
    }
    return result;
}

static int at_least_one(int cost) {
    return cost < 1 ? 1 : cost;
}

// cost = cost after the vanilla modifiers, magister = the hero wears the Ring of the Magister (replaces its
// vanilla halving)
static int __stdcall mana_cost(IHero* hero, int* spell, int cost, int magister) {
    if (magister) cost = at_least_one(cost - 4);
    if (hero->has_artifact(ARTIFACT_GENJIS_HAT)) cost = at_least_one(cost - 1);
    if (hero->has_artifact(ARTIFACT_MOONLIGHT_COAT)) cost -= cost / 4;

    switch (get_spell_element(spell)) {
    case ELEMENT_WATER:
        if (hero->skill_mastery(PERK_FROST_POWER)) cost = ninety_percent(cost);
        if (hero->has_artifact(ARTIFACT_CROWN_OF_THE_FROST_LORD)) cost = at_least_one(cost / 2);
        break;
    case ELEMENT_FIRE:
        if (hero->skill_mastery(PERK_BLAZE_POWER)) cost = ninety_percent(cost);
        if (hero->has_artifact(ARTIFACT_BLAZING_SPELLBOOK)) cost = at_least_one(((cost * 5) >> 3) + (cost >> 3));
        break;
    case ELEMENT_AIR:
        if (hero->skill_mastery(PERK_THUNDER_POWER)) cost = ninety_percent(cost);
        break;
    case ELEMENT_EARTH:
        if (hero->skill_mastery(PERK_EARTH_POWER)) cost = ninety_percent(cost);
        break;
    }

    switch (get_spell_school(spell)) {
    case SPELL_SCHOOL_NATURAL:
        if (hero->has_artifact(ARTIFACT_SYLVAN_AMULET)) cost = at_least_one(cost - 3);
        break;
    case SPELL_SCHOOL_DARK:
        if (hero->has_artifact(ARTIFACT_DEADWOOD_STAFF)) cost = at_least_one(cost - 1);
        break;
    }
    return cost;
}

// ebx = casting IHero, edi = spell, esi = mana cost, eax = Ring of the Magister count (vanilla halving replaced)
__declspec(naked) void ManaCostFork() {
    __asm
    {
        push eax
        push esi
        push edi
        push ebx
        call mana_cost
        mov esi, eax
        mov dword ptr ss: [esp + 0x10], esi
        jmp [ManaCost_return]
    }
}
