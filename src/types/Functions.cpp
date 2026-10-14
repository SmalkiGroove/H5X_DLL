#include "pch.h"

ThiscallInt_IntPtr_Int count_equipped_artifact = (ThiscallInt_IntPtr_Int)0x00CA6680;
ThiscallInt_IntPtr_Int count_backpack_artifact = (ThiscallInt_IntPtr_Int)0x00CA6B30;
ThiscallInt_IntPtr_Int skill_mastery = (ThiscallInt_IntPtr_Int)0x00B4D0B0;
ThiscallInt_IntPtr_Int has_skill = (ThiscallInt_IntPtr_Int)0x00BC32D0;

CdeclIntPtr_Void get_defaultstats = (CdeclIntPtr_Void)0x00BCB3F0;
ThiscallIntPtr_IntPtr get_hero_data = (ThiscallIntPtr_IntPtr)0x00977700;
ThiscallInt_IntPtr check_week_of_balance_luck = (ThiscallInt_IntPtr)0x00BCDD20;

ThiscallInt_SkillId get_skill_data = (ThiscallInt_SkillId)0x00875370;
ThiscallInt_SkillId is_skilltype_skill = (ThiscallInt_SkillId)0x00875380;
ThiscallInt_SkillId is_skilltype_perk = (ThiscallInt_SkillId)0x008753A0;
ThiscallInt_IntPtr get_skill_id = (ThiscallInt_IntPtr)0x009F5C90;
ThiscallInt_IntPtr_Int has_hero_spec = (ThiscallInt_IntPtr_Int)0x00B4B5A0;
ThiscallInt_IntPtr get_skill_mastery = (ThiscallInt_IntPtr)0x00ACFEF0;

ThiscallInt_IntPtr get_spell_id = (ThiscallInt_IntPtr)0x00A34110;
ThiscallInt_IntPtr get_spell_element = (ThiscallInt_IntPtr)0x00A342C0;
ThiscallInt_IntPtr get_spell_school = (ThiscallInt_IntPtr)0x00A34350;
ThiscallInt_IntPtr is_spell_empowered = (ThiscallInt_IntPtr)0x00A34100;
ThiscallInt_IntPtr get_unit_spellproof = (ThiscallInt_IntPtr)0x0097D2A0;
FastcallInt_IntPtr_IntPtr_Int_Int_Int_Int apply_spell_damage = (FastcallInt_IntPtr_IntPtr_Int_Int_Int_Int)0x004E6B60;
// func_ComputeMagicDamage (ret 18h): damage left after the target's resistance to a spell.
// (spell ID used as element reference: 3 Lightning Bolt, 4 Ice Bolt (cold bonus), 5 Fireball (fire bonus); caster (0 for bonus damage),
//  damage, target, 0, estimate (1 = attack prediction, no roll), 1, 0)
FastcallInt_Int_IntPtr_Float_IntPtr_Int_Int_Int_Int compute_magic_damage = (FastcallInt_Int_IntPtr_Float_IntPtr_Int_Int_Int_Int)0x0097D740;
// Spell visual launcher 0x009790C0 (ret 0Ch): ecx = spell ref {0, spell ID, 0, 0, 0}, edx = caster unit, then target unit, time,
// 0x00F495D4. Plays the spell's projectile (CombatUnit +0x144) and impact (+0x148), returns the impact time.
// Spell ID 0xF7 = the "STORMSTRIKE" bolt (ABILITY_STORMSTRIKE, 0x00A60964).
FastcallInt_IntPtr_IntPtr_IntPtr_Int_Int launch_spell_visual = (FastcallInt_IntPtr_IntPtr_IntPtr_Int_Int)0x009790C0;
ThiscallVoid_IntPtr destroy_spell_ref = (ThiscallVoid_IntPtr)0x0041A3C0;  // spell ref destructor (frees +0x8, releases +0x0)
ThiscallIntPtr_Int get_spell_record = (ThiscallIntPtr_Int)0x008496A0;  // func_GetSpellRecord: ecx = spell ID -> NDb::SSpell* (Spell.h), 0 if unresolved
// Damage spell pipeline, in func_CastSpell order (case 0x0097DE65: Magic Arrow, Lightning Bolt, ...):
// func_PrepareSpellDamageParams (ret 0Ch): base damage (spell ID, spellpower, mastery, combat vtable +0x3C result,
//   0): "damage" table (Earthquake: "duration" table, Earthquake.cpp), empowered x, week of the element.
FastcallInt_Int_Int_Int_IntPtr_IntPtr prepare_spell_damage = (FastcallInt_Int_Int_Int_IntPtr_IntPtr)0x00976B90;
// func_GetSpellDamage (ret 18h): damage after the caster's bonuses and the target's immunities / resistance / spell
//   proof (damage, spell ref {handle, spell ID, ...}, caster, target, 1, time, multiplier (cast context +0x34), 1).
FastcallInt_Int_IntPtr_IntPtr_IntPtr_Int_Int_Float_Int get_spell_damage = (FastcallInt_Int_IntPtr_IntPtr_IntPtr_Int_Int_Float_Int)0x0097D600;
// func_ComputeElementalChainDmg (ret 10h): extra elemental damage against the target's element ("FLYING_SIGN_ELEMENTAL_DAMAGE"),
//   added to the damage (time, damage, caster, target, spell ID, 0).
FastcallInt_Int_Int_IntPtr_IntPtr_Int_Int compute_elemental_spell_damage = (FastcallInt_Int_Int_IntPtr_IntPtr_Int_Int)0x00977E60;
// func_IsAllyOfCaster: target (edx) belongs to the side controlled by the caster (ecx) and is not under the other
//   side's control (func_IsUnitControlledByOtherSide: hypnotized).
FastcallByte_IntPtr_IntPtr is_ally_of_caster = (FastcallByte_IntPtr_IntPtr)0x004E5750;

ThiscallIntPtr_Int get_creature_data = (ThiscallIntPtr_Int)0x0089FBF0;
ThiscallInt_IntPtr get_creature_town = (ThiscallInt_IntPtr)0x004BA910;  // ICreature* -> faction (creature data +0x98)
FastcallByte_IntPtr func_GetCreatureFlying = (FastcallByte_IntPtr)0x004BA950;
ThiscallInt_IntPtr get_unit_luck_clamped = (ThiscallInt_IntPtr)0x004BB3C0;
FastcallUint_Int_IntPtr_Char get_damage_stack_count = (FastcallUint_Int_IntPtr_Char)0x00A6BD90;
ThiscallPtrPtr_Void construct_creature_lightning_fx_event = (ThiscallPtrPtr_Void)0x00C9A620;
FastcallVoid_IntPtr_IntPtr_Int_Int_Int_Int_Int collect_adjacent_units = (FastcallVoid_IntPtr_IntPtr_Int_Int_Int_Int_Int)0x00B3D560;
FastcallInt_IntPtr_IntPtr_Int_Int get_unit_adjacency_roll = (FastcallInt_IntPtr_IntPtr_Int_Int)0x00A59990;
// func_CollectAllCombatUnits (ecx = combat, edx = engine vector {begin, end, capacity}, freed first): units of both
// sides (combat vtable +0x84(side) -> side vtable +0x6C). Entries are not CombatUnit subobjects: unit = entry + 4 +
// [[entry + 4] + 8] (Earthquake.cpp).
FastcallVoid_IntPtr_IntPtr collect_all_combat_units = (FastcallVoid_IntPtr_IntPtr)0x00AF55E0;

CdeclPtr_Uint engine_alloc = (CdeclPtr_Uint)0x00878FD0;
CdeclVoid_Ptr engine_free = (CdeclVoid_Ptr)0x00879240;

FastcallVoid_IntPtr_Char_3Int notify_spec_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB930;
FastcallVoid_IntPtr_Char_3Int notify_skill_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB960;
FastcallVoid_IntPtr_Char_3Int notify_artifact_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB9C0;

FastcallVoid_Int enqueue_unit_refresh = (FastcallVoid_Int)0x0050C020;

// --- DLL helpers ---

// Adventure map hero of the side owning the unit (nullptr without hero).
// unit (vtable +0x0C) -> side, side (vtable +0x0C) -> combat hero, combat hero (vtable +0x00) -> IHero.
IHero* owner_hero(ICombatUnit* unit) {
	int* side = unit->unit_ref();
	if (!side) return nullptr;
	int* combat_hero = ((GetPtr)(*(int***)side)[0x0C / 4])(side);
	if (!combat_hero) return nullptr;
	return (IHero*)((GetPtr)(*(int***)combat_hero)[0])(combat_hero);
}
