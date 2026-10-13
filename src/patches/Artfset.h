#pragma once

// Abilities granted by artifact sets. Called by the creature has_ability hook ArtfsetAbilityFork in ModAbilities.cpp
// for every ability query that reaches the creature's own ability list; return non-zero to grant the ability.
// unit = creature CombatUnit subobject.

int artfset_inferno_ability(ICombatUnit* unit, int ability);
int artfset_necro_ability(ICombatUnit* unit, int ability);
int artfset_dungeon_ability(ICombatUnit* unit, int ability);
int artfset_dwarven_ability(ICombatUnit* unit, int ability);

// Stat bonuses of artifact sets, called by the stat hooks of other patches.

// Academy (15), 4 pieces: the hero's caster creatures have twice their mana and spellpower (ModCasterCreatures.cpp).
bool artfset_academy_casters(IHero* hero);
// Academy (15), 4 pieces + Academy class: flat health added to each of the hero's mages (ModHealth.cpp).
int artfset_academy_health(IHero* hero, ICreature* creature);
// Sylvan (12) / Dungeon (16), 4 pieces: the hero's spellpower is doubled for nature / dark magic spells
// (ModEffectiveSpellpower.cpp). spell = spell ref (not null).
bool artfset_sylvan_spellpower_doubled(IHero* hero, int* spell);
bool artfset_dungeon_spellpower_doubled(IHero* hero, int* spell);
// Dungeon (16), 6 pieces: the unit's hero makes it immune to Curse of the Netherworld (UnholyWordImmunitiesFix.cpp).
bool artfset_dungeon_unholy_word_immune(ICombatUnit* unit);
// Dungeon (16), 6 pieces: flat initiative of the hero's creatures (ModInitiative.cpp).
float artfset_dungeon_initiative(IHero* hero);
// Dwarven (17), 4 pieces: 1 extra source of Offensive / Defensive Formation bonus (OffensiveDefensiveFormation.cpp).
int artfset_dwarven_formation(IHero* hero);
// Dwarven (17), 6 pieces: the hero's creatures take 50% less spell damage (ModSpellProof.cpp).
bool artfset_dwarven_spell_proof(IHero* hero);
