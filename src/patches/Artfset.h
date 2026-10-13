#pragma once

// Abilities granted by artifact sets. Called by the creature has_ability hook ArtfsetAbilityFork in ModAbilities.cpp
// for every ability query that reaches the creature's own ability list; return non-zero to grant the ability.
// unit = creature CombatUnit subobject.

int artfset_inferno_ability(ICombatUnit* unit, int ability);
int artfset_necro_ability(ICombatUnit* unit, int ability);

// Stat bonuses of artifact sets, called by the stat hooks of other patches.

// Academy (15), 4 pieces: the hero's caster creatures have twice their mana and spellpower (ModCasterCreatures.cpp).
bool artfset_academy_casters(IHero* hero);
// Academy (15), 4 pieces + Academy class: flat health added to each of the hero's mages (ModHealth.cpp).
int artfset_academy_health(IHero* hero, ICreature* creature);
