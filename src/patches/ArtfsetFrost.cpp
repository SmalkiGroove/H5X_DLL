#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/Spells.h"
#include "Artfset.h"

// FROST ARTIFACT SET (21)
// 4: ice spells (ELEMENT_WATER) cast by the hero's side are never resisted by enemy units: no resistance roll, no spell
//    proof / resistance multiplier at all, like the Inferno set for fire (hook in ModSpellPenetration.cpp). Immunities
//    still apply.
// 4: enemy creatures start the combat Frozen: buff 0xC9 with the duration of the ice spells' freeze (Master of Ice
//    effect, func_CheckIceSpellFreeze 0x004E6600: DefaultStats +0x7A4 turns). Creatures immune to magic or to water
//    are spared. Applied by the combat start hook of ModStartingEffects.cpp.
// No patch of its own.

static const int frozen_buff = 0xC9;
static const int frost_freeze_turns = 0x7A4;   // DefaultStats float, turns

static int frost_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_FROST) : 0;
}

bool artfset_frost_ignores_resistance(int spell, ICombatUnit* caster, ICombatUnit* target) {
	if (get_spell_element((int*)spell) != ELEMENT_WATER) return false;
	IHero* hero = owner_hero(caster);
	if (frost_pieces(hero) < 4) return false;
	return owner_hero(target) != hero;  // enemy units only (neutrals have no hero)
}

void artfset_frost_starting_freeze(ICombatUnit* unit, ICombatHero* enemy, int time) {
	IHero* hero = enemy->hero();
	if (frost_pieces(hero) < 4) return;
	if (unit->has_ability(ABILITY_IMMUNITY_TO_MAGIC) || unit->has_ability(ABILITY_IMMUNITY_TO_WATER)) return;

	int duration = (int)(turn_duration() * *(float*)((char*)get_defaultstats() + frost_freeze_turns));
	// Same call as func_CheckIceSpellFreeze: (buff, duration, 0, caster level, spellpower, time, caster)
	unit->add_timed_effect(frozen_buff, duration, 0, enemy->hero_level(), hero->spellpower(), time, enemy);
}
