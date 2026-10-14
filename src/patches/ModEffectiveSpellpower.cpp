#include "pch.h"
#include "types/Spells.h"
#include "Artfset.h"

// ADD BONUS SPELLPOWER FROM ADDITIONAL SOURCES
// Skill Blaze Power (231) : +3 for fire spells
// Skill Frost Power (232) : +3 for ice spells
// Skill Thunder Power (233) : +3 for lightning spells
// Skill Earth Power (234) : +3 for earth spells

// MULTIPLY THE FINAL SPELLPOWER (after every flat bonus and buff, before the >= 1 clamp)
// Sylvan set (12), 4 pieces : x2 for nature spells (ArtfsetPreserve.cpp)
// Dungeon set (16), 4 pieces : x2 for dark spells (ArtfsetDungeon.cpp)
// Academy set (15), 4 pieces : +2% per knowledge point, every spell and the spellpower shown without a spell
//   (ArtfsetAcademy.cpp)

void EffectiveSpellpowerFork();
void SpellpowerMultiplierFork();

int EffectiveSpellpower_fork = 0x00BC0582;    // end of the hero spellpower for a spell, before buff 0x14D: mov ecx,[esi+4] / mov edx,[ecx+8]
int EffectiveSpellpower_return = 0x00BC0588;
int SpellpowerMultiplier_fork = 0x00BC05A2;    // end of the hero spellpower for a spell, after buff 0x14D: cmp ebx,1 / mov eax,ebx
int SpellpowerMultiplier_return = 0x00BC05A7;  // jg (flags of the cmp)

void EffectiveSpellpower_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, EffectiveSpellpower_fork, 6, EffectiveSpellpowerFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, SpellpowerMultiplier_fork, 5, SpellpowerMultiplierFork, 0, 0, 0 });
}

// Element power perk matching the spell's element.
static int element_power_perk(int spell) {
    switch (get_spell_element((int*)spell)) {
    case ELEMENT_AIR:   return PERK_THUNDER_POWER;
    case ELEMENT_FIRE:  return PERK_BLAZE_POWER;
    case ELEMENT_WATER: return PERK_FROST_POWER;
    case ELEMENT_EARTH: return PERK_EARTH_POWER;
    }
    return 0;
}

// Spellpower added by the element power perks. Like the vanilla bonuses of this function, it stores the new
// value in the game's copy (spellpower_slot) and adds the perk to the tooltip (notify_skill_buff).
static int __fastcall element_power_bonus(ICombatHero* hero, int spell, int spellpower, int* notify_context, int* spellpower_slot) {
    if (!spell) return 0;  // no spell context
    int perk = element_power_perk(spell);
    if (!perk || !hero->has_perk(perk)) return 0;
    *spellpower_slot = spellpower + 3;
    notify_skill_buff(notify_context, 1, perk, 0, 0);
    return 3;
}

// Final spellpower for the spell. hero_unit = hero CombatUnit subobject, spell = spell ref (0 without spell context).
static int __fastcall multiplied_spellpower(ICombatUnit* hero_unit, int* spell, int spellpower) {
    IHero* hero = owner_hero(hero_unit);
    if (spell && (artfset_sylvan_spellpower_doubled(hero, spell) || artfset_dungeon_spellpower_doubled(hero, spell)))
        spellpower *= 2;
    return artfset_academy_hero_spellpower(hero, spellpower);
}

// esi = hero complete object (vbptr at esi+4), edi = spell (0 without spell context), ebx = spellpower,
// ebp = notify context, [esp+0x10] = the game's copy of the spellpower
__declspec(naked) void EffectiveSpellpowerFork() {
    __asm
    {
        lea eax, dword ptr [esp + 0x10]
        push eax                                // spellpower_slot
        push ebp                                // notify_context
        push ebx                                // spellpower
        mov edx, edi                            // spell
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        lea ecx, dword ptr [ecx + esi + 0x4]    // ICombatHero (CombatUnit subobject)
        call element_power_bonus
        add ebx, eax

        mov ecx, dword ptr [esi + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        jmp [EffectiveSpellpower_return]
    }
}

// esi = hero CombatUnit subobject (lea esi,[edx+esi+4] at 0x00BC058C), edi = spell ref (0 without spell),
// ebx = spellpower. Replays cmp ebx,1 / mov eax,ebx for the clamp after the return.
__declspec(naked) void SpellpowerMultiplierFork() {
    __asm
    {
        push ebx                                // spellpower
        mov edx, edi
        mov ecx, esi
        call multiplied_spellpower
        mov ebx, eax

        cmp ebx, 0x1
        mov eax, ebx
        jmp [SpellpowerMultiplier_return]
    }
}
