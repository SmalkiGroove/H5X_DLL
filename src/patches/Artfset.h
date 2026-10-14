#pragma once

// Abilities granted by artifact sets. Called by the creature has_ability hook ArtfsetAbilityFork in ModAbilities.cpp
// for every ability query that reaches the creature's own ability list; return non-zero to grant the ability.
// unit = creature CombatUnit subobject.

int artfset_inferno_ability(ICombatUnit* unit, int ability);
int artfset_necro_ability(ICombatUnit* unit, int ability);
int artfset_dungeon_ability(ICombatUnit* unit, int ability);
int artfset_dwarven_ability(ICombatUnit* unit, int ability);
int artfset_orcs_ability(ICombatUnit* unit, int ability);
int artfset_bandit_ability(ICombatUnit* unit, int ability);

// Stat bonuses of artifact sets, called by the stat hooks of other patches.

// Inferno (13), 4 pieces + Inferno class / Frost (21), 4 pieces: fire / ice spells of the hero's side are never resisted
// by enemy units (ModSpellPenetration.cpp). spell = spell ID, caster / target = CombatUnit subobjects (not null).
bool artfset_inferno_ignores_resistance(int spell, ICombatUnit* caster, ICombatUnit* target);
bool artfset_frost_ignores_resistance(int spell, ICombatUnit* caster, ICombatUnit* target);
// Frost (21), 4 pieces: the enemy hero freezes the creature at combat start (ModStartingEffects.cpp).
// unit = creature CombatUnit, enemy = the combat hero of the other side (not null), time = combat start time.
void artfset_frost_starting_freeze(ICombatUnit* unit, ICombatHero* enemy, int time);
// Spirit (26), 5 pieces: Sorrow for the whole combat on the creature at combat start (ModStartingEffects.cpp, same
// arguments as above), and the attack / defense multiplier of the enemy creatures (ModAttack.cpp / ModDefense.cpp;
// enemy_hero = the hero facing the creature).
void artfset_spirit_starting_sorrow(ICombatUnit* unit, ICombatHero* enemy, int time);
float artfset_spirit_enemy_stat_multiplier(IHero* enemy_hero);

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
// Bandit (28), 5 pieces: speed of the hero's creatures (ModSpeed.cpp).
int artfset_bandit_speed(IHero* hero);
// Orcs (18), 4 / 6 pieces + Stronghold class: rage points of the hero's creatures at combat start (ModStartingRage.cpp).
int artfset_orcs_starting_rage(IHero* hero);
