#include "pch.h"
#include "types/Artifacts.h"
#include "types/Skills.h"
#include "types/Spells.h"
#include "Artfset.h"

// SPIRIT ARTIFACT SET (26)
// 5: enemy creatures are under Sorrow for the whole combat, as cast by the hero (Dark Magic mastery, spellpower at combat
//    start): the morale / luck penalty of the Sorrow <damage> table (Base by mastery + PerPower x spellpower). Creatures
//    immune to Sorrow (func_ComputeSpellHitChance 100) are spared. Applied by the combat start hook of
//    ModStartingEffects.cpp; replaces the Routine_ArtfsetSpirit script (a timed Sorrow cast).
// 5: enemy creatures have -20% attack and defense (ModAttack.cpp / ModDefense.cpp)
// No patch of its own.

static const float spirit_enemy_stat_multiplier = 0.8f;

static int spirit_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_SPIRIT) : 0;
}

float artfset_spirit_enemy_stat_multiplier(IHero* enemy_hero) {
	return spirit_pieces(enemy_hero) >= 5 ? spirit_enemy_stat_multiplier : 1.0f;
}

void artfset_spirit_starting_sorrow(ICombatUnit* unit, ICombatHero* enemy, int time) {
	IHero* hero = enemy->hero();
	if (spirit_pieces(hero) < 5) return;
	if (spell_hit_chance(unit, SPELL_SORROW, enemy) >= 100) return;
	// Sorrow reads its mastery in the effect value (+0x20) and its spellpower at +0x28 (0x00989DA1 / 0x0098A2F4)
	unit->add_permanent_effect(SPELL_SORROW, hero->skill_mastery(SKILL_DARK_MAGIC), 0, hero->spellpower(), time, enemy);
}
