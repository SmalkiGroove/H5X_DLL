#pragma once

// Abilities granted by artifact sets. Called by the creature has_ability hook ArtfsetAbilityFork in ModAbilities.cpp
// for every ability query that reaches the creature's own ability list; return non-zero to grant the ability.
// unit = creature CombatUnit subobject.

int artfset_inferno_ability(ICombatUnit* unit, int ability);
int artfset_necro_ability(ICombatUnit* unit, int ability);
int artfset_dungeon_ability(ICombatUnit* unit, int ability);
int artfset_dwarven_ability(ICombatUnit* unit, int ability);
int artfset_orcs_ability(ICombatUnit* unit, int ability);

// Stat bonuses of artifact sets, called by the stat hooks of other patches.

// Academy (15), 4 pieces: final spellpower of the hero, +2% per knowledge point (ModEffectiveSpellpower.cpp), and of
// the hero's caster creatures, +2% per maximum mana point (ModCasterCreatures.cpp; unit = creature CombatUnit).
int artfset_academy_hero_spellpower(IHero* hero, int spellpower);
int artfset_academy_caster_spellpower(ICombatUnit* unit, int spellpower);
// Academy (15), 4 pieces + Academy class: the hero's ATB after a cast, 15% of the ATB used saved (ModAtbReset.cpp).
float artfset_academy_hero_cast_atb(IHero* hero, float atb);
// Academy (15), 6 pieces + Academy class: attack damage multiplier of a defending unit, mages and their adjacent
// allies (ModDamageOutput.cpp).
float artfset_academy_damage_taken(ICombatUnit* defender);
// Sylvan (12) / Dungeon (16), 4 pieces: the hero's spellpower is doubled for nature / dark magic spells
// (ModEffectiveSpellpower.cpp). spell = spell ref (not null).
bool artfset_sylvan_spellpower_doubled(IHero* hero, int* spell);
bool artfset_dungeon_spellpower_doubled(IHero* hero, int* spell);
// Dungeon (16), 6 pieces: the unit's hero makes it immune to Curse of the Netherworld (UnholyWordImmunitiesFix.cpp).
bool artfset_dungeon_unholy_word_immune(ICombatUnit* unit);
// Dungeon (16), 6 pieces: flat initiative of the hero's creatures (ModInitiative.cpp).
float artfset_dungeon_initiative(IHero* hero);
// Sylvan (12) / Dungeon (16), 4 pieces: the hero learns the nature / dark spells of tiers 1 to 3 for good. Called after
// every equip change by ArtfsetExpand.cpp.
void artfset_sylvan_teach_spells(IHero* hero);
void artfset_dungeon_teach_spells(IHero* hero);
// Dwarven (17), 4 pieces: 1 extra source of Offensive / Defensive Formation bonus (OffensiveDefensiveFormation.cpp).
int artfset_dwarven_formation(IHero* hero);
// Dwarven (17), 6 pieces: the hero's creatures take 50% less spell damage (ModSpellProof.cpp).
bool artfset_dwarven_spell_proof(IHero* hero);
// Orcs (18), 6 pieces: speed (ModSpeed.cpp) and morale (ModMorale.cpp) of the hero's creatures.
int artfset_orcs_speed(IHero* hero);
int artfset_orcs_morale(IHero* hero);
// Orcs (18), 4 / 6 pieces + Stronghold class: rage points of the hero's creatures at combat start (ModStartingRage.cpp).
int artfset_orcs_starting_rage(IHero* hero);
