#pragma once
#ifndef CREATURE_H
#define CREATURE_H

#include "structs/CombatUnit.h"

// AdvMap army stack unit (vtable 0x00E0F734, NWorld::CCreature, vtordisp -0x14).
// Ctor FUN_0043cfa0 (EngineAlloc 0x24). RTTI .?AVCCreature@NWorld@@ @ 0x00F29C58.
// Stat helpers (func_GetCreatureHealth, func_GetCreatureStats, func_GetCreatureCostResources, …)
// are static DB/hero-context functions — not vtable slots on this interface.
//
// SCreature DB (func_GetCreatureData) — patch-relevant offsets:
//   +0x58 Speed, +0x5C Initiative, +0x60 Flying (bool, .xdb field "Flying")
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
};

// CombatCreature (vtable 0x00E4FBA4, NWorld::CCombatCreature, vtordisp -0x144).
// RE rule: .cursor/rules/h5x-re-combat-creature.mdc (auto-synced).
//
// Subobject on a combat unit (from unit pointer in esi/ebp):
//   mov eax, [unit - 0x144]          ; vbtable
//   mov ecx, [eax + 8]
//   lea ecx, [ecx + unit - 0x144]    ; ICombatCreature*
// Creature type id on the outer unit: [unit + 0x1C].

struct ICombatCreature : ICombatUnit {
	int* creature_ref() {
		return instance->get_unit_ref((int*)this);
	}
	int mana_points() {
		return instance->get_mana((int*)this);
	}
	bool has_ability(int ability_id) {
		return instance->has_ability((int*)this, ability_id) != 0;
	}
};

#endif
