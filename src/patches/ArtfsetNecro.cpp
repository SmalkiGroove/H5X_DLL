#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "Artfset.h"

// NECROPOLIS ARTIFACT SET (14)
// 4: scripted
// 4 Necropolis: scripted
// 6: hero's creatures are undead (ability 10)
// 6 Necropolis: hero's undead creatures (so all of them) have Regeneration (ability 60)
// Both through the shared has_ability hook (ModAbilities.cpp), no patch of its own.

static int necro_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_NECRO) : 0;
}

static bool is_necropolis_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_NECROPOLIS_MIGHT || hero_class == HERO_CLASS_NECROPOLIS_MAGIC;
}

// Abilities 10 / 60, through the shared has_ability hook (ModAbilities.cpp)
int artfset_necro_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_UNDEAD && ability != ABILITY_REGENERATION) return 0;
	IHero* hero = owner_hero(unit);
	if (necro_pieces(hero) < 6) return 0;
	if (ability == ABILITY_UNDEAD) return 1;
	// every creature is undead with 6 pieces, natives and granted alike
	return is_necropolis_class(hero);
}
