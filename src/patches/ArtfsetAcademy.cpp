#include "pch.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "Artfset.h"

// ACADEMY ARTIFACT SET (15)
// 4: hero's caster creatures have twice their mana and spellpower (DefaultStats +0x117C, the vanilla Sar-Issus effect)
// 4 Academy: hero's mages (Mage, Archmage, Battle Mage) have +10 health
// 6: scripted
// No patch of its own: the mana / spellpower sites are hooked by ModCasterCreatures.cpp (shared with the Researcher spec),
// the health by ModHealth.cpp.

enum AcademyMage {
	CREATURE_MAGE = 63,
	CREATURE_ARCH_MAGE = 64,
	CREATURE_BATTLE_MAGE = 162,
};

static const int academy_mage_health = 10;

static int academy_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_ACADEMY) : 0;
}

static bool is_academy_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_ACADEMY_MIGHT || hero_class == HERO_CLASS_ACADEMY_MAGIC;
}

bool artfset_academy_casters(IHero* hero) {
	return academy_pieces(hero) >= 4;
}

int artfset_academy_health(IHero* hero, ICreature* creature) {
	if (academy_pieces(hero) < 4 || !is_academy_class(hero)) return 0;
	switch (creature->type_id) {
	case CREATURE_MAGE:
	case CREATURE_ARCH_MAGE:
	case CREATURE_BATTLE_MAGE:
		return academy_mage_health;
	}
	return 0;
}
