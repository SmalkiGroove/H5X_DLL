#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "Artfset.h"

// BANDIT ARTIFACT SET (28)
// 5: hero's creatures get +1 speed (ModSpeed.cpp) and ABILITY_NO_ENEMY_RETALIATION (shared has_ability hook,
//    ModAbilities.cpp; the attack code reads ability 5 through CombatUnit +0x28C)
// No patch of its own.

static const int bandit5_speed = 1;

static int bandit_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_BANDIT) : 0;
}

// Ability 5, through the shared has_ability hook (ModAbilities.cpp)
int artfset_bandit_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_NO_ENEMY_RETALIATION) return 0;
	return bandit_pieces(owner_hero(unit)) >= 5;
}

int artfset_bandit_speed(IHero* hero) {
	return bandit_pieces(hero) >= 5 ? bandit5_speed : 0;
}
