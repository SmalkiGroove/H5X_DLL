#pragma once
#ifndef CREATURE_H
#define CREATURE_H

#include "structs/CombatUnit.h"
#include "structs/Hero.h"

// AdvMap army stack unit (vtable 0x00E0F734, NWorld::CCreature, vtordisp -0x14).
// Ctor FUN_0043cfa0 (EngineAlloc 0x24). RTTI .?AVCCreature@NWorld@@ @ 0x00F29C58.
// Stat helpers (func_GetCreatureHealth, func_GetCreatureStats, func_GetCreatureCostResources, …)
// are static DB/hero-context functions — not vtable slots on this interface.
//
// SCreature DB (func_GetCreatureData) — patch-relevant offsets:
//   +0x4C Shots, +0x58 Speed, +0x5C Initiative, +0x60 Flying (bool, .xdb field "Flying"), +0x84 Power,
//   +0x8C Tier, +0x98 Town, +0xA8 WeeklyGrowth (>= 1), +0xDC CombatSize (1 or 2), +0xE0 PatternAttack, +0x100 BaseCreatureID
// Flying in combat: vtable is_flying @ CombatUnit +0x190 → func_GetCreatureFlying @ 0x004BA950.
//
// Object size 0x24. Timed-stats blob at +0x2C lives on a base/MI subobject (vtordisp -0x14).
//
// Army container (separate): CArmy vtable 0x00E7FF50; army slot node vtable 0x00E800AC.

enum {
	Creature_vtable_addr = 0x00E0F734,
	Creature_vtordisp = 0x14
};

struct Creature_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	int* call_28;
	int* call_32;
	int* call_36;
	int* call_40;
	int* call_44;
	int* call_48;
	int* call_52;
	int* call_56;
	int* call_60;
	int* call_64;
	int* call_68;
	int* call_72;
	int* call_76;
	int* call_80;
	int* call_84;
	int* call_88;
	int* call_92;
	int* call_96;
	int* call_100;
	int* call_104;
	int* call_108;
	int* call_112;
	int* call_116;
	int* call_120;
	int* call_124;
	int* call_128;
	int* call_132;
	int* call_136;
	int* call_140;
	int* call_144;
	int* call_148;
	int* call_152;
	int* enqueue_refresh; // 0x9C (156) - 0x00589C30, func_Creature_EnqueueRefresh
	int* on_army_changed; // 0xA0 (160) - 0x006917B0, func_Creature_OnArmyChanged
	int* call_164;
	int* call_168;
	int* purecall_stub; // 0xAC (172) - 0x004018B0, _purecall
	int* call_176;
	int* call_180;
	int* call_184;
	int* call_188;
	int* call_192;
	int* call_196;
	int* call_200;
	int* call_204;
	int* call_208;
	int* call_212;
	int* call_216;
	GetCreatureCostResources get_cost_resources; // 0xE0 (224) - 0x00458770, wraps db tuple @ creature+0xAC
	int* get_upgrade_cost; // 0xE4 (228) - 0x00458780
	int* get_resource_tuple_a; // 0xE8 (232) - 0x00458790
	int* get_resource_tuple_b; // 0xEC (236) - 0x004587A0
	int* get_resource_tuple_c; // 0xF0 (240) - 0x004587B0
	int* get_resource_tuple_d; // 0xF4 (244) - 0x004587C0
	int* apply_market_step; // 0xF8 (248) - 0x00C242C0, func_Creature_ApplyMarketStep
	int* get_resource_tuple_e; // 0xFC (252) - 0x004587D0
	int* call_256;
	int* get_resource_tuple_f; // 0x104 (260) - 0x004587E0
	int* get_resource_tuple_g; // 0x108 (264) - 0x004587F0
	int* get_resource_tuple_h; // 0x10C (268) - 0x00458800
	int* get_resource_tuple_i; // 0x110 (272) - 0x00458810
	int* get_owner_ref; // 0x114 (276) - 0x00A749C0
	int* notify_refresh; // 0x118 (280) - 0x00456E70
	int* get_timed_stats; // 0x11C (284) - 0x00458820
	int* copy_timed_stats; // 0x120 (288) - 0x00458830
	int* call_292;
	int* scalar_deleting_dtor; // 0x128 (296) - 0x00458760
	int* call_300;
	int* clone_creature; // 0x130 (304) - 0x004584F0
	int* call_308;
	int* call_312;
	int* call_316;
};

struct ICreature {
	Creature_vtable* instance;
	char pad_04[0x18];
	int type_id;   // +0x1C, NDb::SCreature id (arg to func_GetCreatureData)
	int count;     // +0x20, stack size

	void set_count(int n) {
		SetCreatureCount set_count_fn = (SetCreatureCount)0x004BA8F0;
		set_count_fn((int*)this, n);
	}
	void cost_resources(int* out_tuple) {
		instance->get_cost_resources((int*)this, out_tuple);
	}
	// Base creature of the upgrade line (creature data +0x100, 0 for a base creature): own id for a base creature.
	int base_type_id() {
		int base = get_creature_data(type_id)[0x100 / 4];
		return base ? base : type_id;
	}
};

// Combat stack interface (RTTI NWorld::ICombatCreature, vtable 0x00E4FE9C, 35 slots), subobject at +0xA0 of the
// CCombatCreature complete object. Not the repo's ICombatCreature below, which is the CombatUnit (ICombatBase)
// subobject at +0x148. Reached with CombatUnit vtable +0x70 get_combat_stack (0 for heroes; the hero's counterpart
// at +0xA0 is NWorld::ICombatHero, vtable 0x00E84CC4, whose slot 0 returns the IHero).
// From this subobject: vbptr at this-0x9C (CombatUnit = this-0x9C + [[this-0x9C]+8]), complete object at this-0xA0.
// Fields (offsets from this subobject; complete object = +0xA0):
//   +0x08 ICreature* (army stack: type_id, count)       +0x0C base count (see get_base_count)
//   +0x10 stack state history (vector of 0x10-byte entries at +0x1C/+0x20; last entry: +0x0 time, +0x4 current
//         count, +0x8 used by slot 0x44); empty history reads zeros at 0x00FEFAC8
//   +0x2C dead flag (cleared by add_creatures)          +0x2D defending (func_GetUnitDefense reads complete +0xCD)
//   +0x44 / +0x45 flags (slots 0x18 / 0x14)              +0x48 magic element
//   +0x4C status flags (0x2000 = Horror of the Death)    +0x58 float, max HP reduction fraction (slots 0x4C / 0x54)
//   +0x80 Treachery triggered                            +0x84 rage points (thresholds: slot 0x80)
//   +0x88 alternate stats object (slot 0x1C returns 8, slot 0x50 uses FUN_004BCD60)
// "current count" = last history entry +0x4.
enum {
	CombatStack_vtable_addr = 0x00E4FE9C,
	CombatStack_subobject_offset = 0xA0
};

struct CombatStack_vtable {
	GetPtr get_creature; // 0x00 (0) - 0x00724930, returns [this+0x8] (ICreature*)
	int* get_pair_30; // 0x04 (4) - 0x008AE070, thiscall(int* out), ret 4: out[0] = [this+0x30], out[1] = [this+0x34]
	GetInt get_base_count; // 0x08 (8) - 0x0081BF70, returns [this+0xC]: stack size used for max values; lowered by remove_creatures, raised by add_creatures
	GetInt get_base_total_health; // 0x0C (12) - 0x008A1DA0 FUN_008a1da0, func_GetCreatureHealth(owner hero, creature) * base count
	GetInt get_current_value_64; // 0x10 (16) - 0x008A41F0 FUN_008a41f0, current count * creature data +0x80 (or +0x64 when 0), FUN_004BAAD0
	int* get_flag_45; // 0x14 (20) - 0x008AE0B0, returns byte [this+0x45], set by add_creatures when the stack was dead
	int* get_flag_44; // 0x18 (24) - 0x008AE0C0, returns byte [this+0x44]; when set, func_GetUnitAttack and func_GetUnitDefense double the stat
	GetInt get_tier; // 0x1C (28) - 0x008A2110, 8 when [this+0x88] is set, else creature data +0x8C (FUN_004BA900, tier)
	GetInt get_base_power; // 0x20 (32) - 0x008A1E30 FUN_008a1e30, func_GetCreaturePowerValue (creature data +0x84, >= 1) * base count
	GetInt get_current_power; // 0x24 (36) - 0x008A54F0, power value * current count (army strength of the stack)
	int* fill_list_28; // 0x28 (40) - 0x008A5650 FUN_008a5650, thiscall(vector* out): clears out, refills it from the creature (FUN_00B61EB0, or FUN_00B61C20 with [this+0x88])
	GetInt get_value_20; // 0x2C (44) - 0x00CBCA20, returns [this+0x20]
	CheckValue set_value_20; // 0x30 (48) - 0x008AE0D0, [this+0x20] = arg, enqueues a unit refresh
	GetInt get_value_24; // 0x34 (52) - 0x00CE6750, returns [this+0x24]
	CheckValue set_value_24; // 0x38 (56) - 0x008AE0F0, [this+0x24] = arg, enqueues a unit refresh
	int* unknown_3c; // 0x3C (60) - 0x008A7EF0 FUN_008a7ef0, thiscall(uint time, int x): calls CombatUnit +0x1F8(time, x, 1), then records a history entry
	int* unknown_40; // 0x40 (64) - 0x008A8050 FUN_008a8050, thiscall(uint time): records a history entry (top creature hp * base count)
	int* unknown_44; // 0x44 (68) - 0x008A3DB0, thiscall(arg): CombatUnit +0x1F4(arg, last entry +0x8 * (1 - [this+0x58]))
	int* add_creatures; // 0x48 (72) - 0x008A7D40 FUN_008a7d40, thiscall(uint time, int count): revives a dead stack, raises base count to current + count (resurrection)
	int* add_health_reduction; // 0x4C (76) - 0x008A8390, thiscall(uint time, float fraction): [this+0x58] = clamp([this+0x58] + fraction, 0, 0.99) (fraction of each creature's max HP removed, CombatUnit +0x1A8), then FUN_008A8260 adjusts the top creature's HP
	GetInt get_creature_health; // 0x50 (80) - 0x008A2130 FUN_008a2130, hp of one creature: func_GetCreatureHealth (or FUN_004BCD60 with [this+0x88]) + FUN_008D9390; buff 0x147 replaces it with a DefaultStats +0x39C based value
	int* get_health_reduction; // 0x54 (84) - 0x008AE090, returns float [this+0x58] (st0)
	GetInt get_magic_element; // 0x58 (88) - 0x00CE6790, returns [this+0x48]
	CheckValue set_magic_element; // 0x5C (92) - 0x008A2A70 FUN_008a2a70, [this+0x48] = arg, logs ": magic element changed to"
	int* unknown_60; // 0x60 (96) - 0x008A2B10, FUN_004BCB50(creature, &[this+0x5C])
	int* remove_creatures; // 0x64 (100) - 0x008A1410 FUN_008a1410, thiscall(int count, uint time): kills count creatures (CombatUnit +0x1F4 with hp * count), base count -= count; stack death at 0
	int* check_treachery; // 0x68 (104) - 0x008A0B60 FUN_008a0b60, ABILITY_TREACHERY (107) once per combat ([this+0x80]): fires when the count ratio drops under a DefaultStats threshold
	int* is_treachery_triggered; // 0x6C (108) - 0x007ABDD0, returns byte [this+0x80]
	int* get_flag_m4; // 0x70 (112) - 0x008AE0A0, returns byte [this-0x4] (complete +0x9C)
	int* refresh_horror_flag; // 0x74 (116) - 0x008A5570 FUN_008a5570, non-undead: sets flag 0x2000 in [this+0x4C] when an enemy has ABILITY_HORROR_OF_THE_DEATH (137)
	GetInt get_value_54; // 0x78 (120) - 0x00C61AC0, returns [this+0x54]
	CheckValue add_rage; // 0x7C (124) - 0x008A2C20 FUN_008a2c20, rage gain for rage users (FUN_008A16B0: creature town 10): scaled by buffs 0x11/0x12/0x14/0x148, hero skill 0xB8, hero buff 0xCC
	CheckValue get_rage_threshold; // 0x80 (128) - 0x008A2B20, arg = rage level 0..3: DefaultStats +0x400 table, scaled down when the hero's skill 0xAC mastery > 3
	int* update_rage; // 0x84 (132) - 0x008A84E0 FUN_008a84e0, resets [this+0x84] when CombatUnit +0x5C, else recomputes the rage level and its effects
	int* apply_rage_level; // 0x88 (136) - 0x008A1760, rage level (FUN_008A1640: [this+0x84] against the thresholds) -> level effects
};

struct ICombatStack {
	CombatStack_vtable* instance;

	ICreature* creature() {
		return (ICreature*)instance->get_creature((int*)this);
	}
	int base_count() {
		return instance->get_base_count((int*)this);
	}
	int creature_health() {
		return instance->get_creature_health((int*)this);
	}
	int current_power() {
		return instance->get_current_power((int*)this);
	}
	int magic_element() {
		return instance->get_magic_element((int*)this);
	}
	// Removes fraction (0..0.99 in total) of each creature's max HP, at combat time `time`.
	void add_health_reduction(int time, float fraction) {
		typedef void(__thiscall* AddHealthReductionFn)(int*, int, float);
		((AddHealthReductionFn)instance->add_health_reduction)((int*)this, time, fraction);
	}
};

// CombatCreature (vtable 0x00E4FBA4, NWorld::CCombatCreature, subobject +0x148, vbptr at this-0x144).
// RE rule: .cursor/rules/h5x-re-combat-creature.mdc (auto-synced).
//
// Subobject on a combat unit (from unit pointer in esi/ebp):
//   mov eax, [unit - 0x144]          ; vbtable
//   mov ecx, [eax + 8]
//   lea ecx, [ecx + unit - 0x144]    ; ICombatCreature*
// Creature type id on the outer unit: [unit + 0x1C].
//
// owner()  — friendly side hero (ModLuck LuckToggleFork @ 0x008A3F0F, ModAbilities @ 0x008A3759).
// opponent() — enemy side hero (ModAttack CreatureAttackFork @ 0x008A4B5C).

namespace combat_creature_detail {

// Combat stack root (ModAttack esi / GetUnitAttack ECX). At is_flying hook 0x008A22D0,
// ECX is already this pointer after the vtordisp thunk @ 0x008AEB60 — not at +0xA0.
inline char* combat_stack_root(ICombatUnit* unit) {
	return (char*)unit;
}

inline ICombatUnit* combat_unit_mi(char* base) {
	if (!base)
		return nullptr;
	int** vbtable = *(int***)((char*)base - CombatCreature_vbptr_offset);
	if (!vbtable)
		return nullptr;
	return (ICombatUnit*)((char*)base - CombatCreature_vbptr_offset + (uintptr_t)vbtable[2]);
}

inline int* call_unit_ref(int* obj) {
	if (!obj)
		return nullptr;
	int** vt = *(int***)obj;
	if (!vt)
		return nullptr;
	return ((GetPtr)vt[3])(obj);
}

inline ICombatHero* combat_hero_from_outer(int* outer) {
	if (!outer)
		return nullptr;
	int** vbtable = *(int***)((char*)outer + 4);
	if (!vbtable)
		return nullptr;
	return (ICombatHero*)((char*)outer + (uintptr_t)vbtable[2] + 4);
}

// ModAttack uses eax after notify_owner (+0x10) as the next this pointer.
inline int* call_notify_owner_side(ICombatUnit* unit) {
	if (!unit || !unit->instance)
		return nullptr;
	return ((ThiscallIntPtr_IntPtr)unit->instance->notify_owner)((int*)unit);
}

} // namespace combat_creature_detail

struct ICombatCreature : ICombatUnit {
	int* creature_ref() {
		return instance->get_unit_ref((int*)this);
	}
	// Max HP of one creature, after the stack's HP reduction (ICombatStack::add_health_reduction)
	int hit_points() {
		return instance->get_creature_max_hp((int*)this);
	}
	int mana_points() {
		return instance->get_mana((int*)this);
	}
	bool has_ability(int ability_id) {
		return instance->has_ability((int*)this, ability_id) != 0;
	}
	// Combat stack interface (CombatUnit vtable +0x70), nullptr when the unit is a hero.
	ICombatStack* stack() {
		return (ICombatStack*)instance->get_combat_stack((int*)this);
	}
	// Army stack (type_id, count) of the unit, nullptr when the unit is a hero.
	ICreature* creature() {
		ICombatStack* combat_stack = stack();
		return combat_stack ? combat_stack->creature() : nullptr;
	}
	// Current count / weekly growth (creature data +0xA8, >= 1): stack size in "weeks of growth"; 0 for a hero.
	float growth_ratio() {
		ICreature* army_stack = creature();
		if (!army_stack) return 0.0f;
		int growth = get_creature_data(army_stack->type_id)[0xA8 / 4];  // WeeklyGrowth
		return (float)instance->get_current_count((int*)this) / (growth > 0 ? growth : 1);
	}
	// Friendly side ICombatHero: 2x get_unit_ref on MI subobject (no notify_owner).
	ICombatHero* owner() {
		using namespace combat_creature_detail;
		ICombatUnit* unit = combat_unit_mi(combat_stack_root(this));
		if (!unit || !unit->instance) return nullptr;
		int* sideRef = call_unit_ref((int*)unit);
		int* heroOuter = call_unit_ref(sideRef);
		return combat_hero_from_outer(heroOuter);
	}
	// Enemy side ICombatHero: notify_owner (+0x10) then get_unit_ref (ModAttack chain).
	ICombatHero* opponent() {
		using namespace combat_creature_detail;
		ICombatUnit* unit = combat_unit_mi(combat_stack_root(this));
		if (!unit || !unit->instance) return nullptr;
		int* sideObj = call_notify_owner_side(unit);
		int* heroOuter = call_unit_ref(sideObj);
		return combat_hero_from_outer(heroOuter);
	}
};

#endif
