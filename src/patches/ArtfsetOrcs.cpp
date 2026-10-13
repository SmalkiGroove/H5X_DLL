#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "Artfset.h"

// ORCS ARTIFACT SET (18)
// 4: hero's creatures have ABILITY_LIZARD_BITE (Beat-up in H5X) (shared has_ability hook, ModAbilities.cpp)
// 4 Stronghold: hero's orcs start the combat with 100 rage points (ModStartingRage.cpp)
// 6: hero's creatures get +2 speed (ModSpeed.cpp) and +2 morale (ModMorale.cpp, added to the hero's morale)
// 6 Stronghold: the starting rage points are 250 instead of 100
// No patch of its own.

static const int orcs4_starting_rage = 100;
static const int orcs6_starting_rage = 250;
static const int orcs6_speed = 2;
static const int orcs6_morale = 2;

static int orcs_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_ORCS) : 0;
}

static bool is_stronghold_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_STRONGHOLD_MIGHT || hero_class == HERO_CLASS_STRONGHOLD_MAGIC;
}

// Ability 58, through the shared has_ability hook (ModAbilities.cpp)
int artfset_orcs_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_LIZARD_BITE) return 0;
	return orcs_pieces(owner_hero(unit)) >= 4;
}

int artfset_orcs_speed(IHero* hero) {
	return orcs_pieces(hero) >= 6 ? orcs6_speed : 0;
}

int artfset_orcs_morale(IHero* hero) {
	return orcs_pieces(hero) >= 6 ? orcs6_morale : 0;
}

int artfset_orcs_starting_rage(IHero* hero) {
	int pieces = orcs_pieces(hero);
	if (pieces < 4 || !is_stronghold_class(hero)) return 0;
	return pieces >= 6 ? orcs6_starting_rage : orcs4_starting_rage;
}
