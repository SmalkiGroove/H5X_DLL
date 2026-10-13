#pragma once

// Abilities granted by artifact sets. Called by the creature has_ability hook ArtfsetAbilityFork in ModAbilities.cpp
// for every ability query that reaches the creature's own ability list; return non-zero to grant the ability.
// unit = creature CombatUnit subobject.

int artfset_inferno_ability(ICombatUnit* unit, int ability);
int artfset_necro_ability(ICombatUnit* unit, int ability);
