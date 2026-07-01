#pragma once
#ifndef SPELL_H
#define SPELL_H

#include "types/Functions.h"

// NDb::SSpell — spell DB record (union struct).
//   RTTI .?AUSSpell@NDb@@ @ 0x00F43B10
//   Base NDb::CResource — RTTI .?AVCResource@NDb@@ @ 0x00F2745C
//
// Runtime resolve: SpellDbRef → FUN_0084da30 / thunk FUN_008496a0 → SSpell*.
// Cast-time getters (func_GetSpellID, func_GetSpellElement, …) take spell id in ECX
// after mass/empowered remapping; they internally resolve SSpell via SpellDbRef context.
//
// Combat stack spellRef (5 dwords): see SpellRef below — used by GetEffectiveSpellPower,
// func_GetSpellDamage, func_ComputeSpellDamage.

// --- Spell school @ SSpell+0x88 (func_GetSpellSchool; returns 8 if unresolved) ---

enum SPELL_SCHOOL {
	SPELL_SCHOOL_UNKNOWN = 8,
};

// --- Element @ SSpell+0xD4 when +0xD8 != 0 (func_GetSpellElement) ---

enum SPELL_ELEMENT {
	SPELL_ELEMENT_NONE = 0,
	SPELL_ELEMENT_FIRE = 1,
	SPELL_ELEMENT_LIGHTNING = 2,
	SPELL_ELEMENT_ICE = 3,
	SPELL_ELEMENT_EARTH = 4,
};

// --- Targeting class @ SSpell+0xD0 ---

enum SPELL_TARGET_CLASS {
	SPELL_TARGET_SINGLE = 0,
	SPELL_TARGET_AREA = 1,
	SPELL_TARGET_GLOBAL = 2,
};

// --- NDb::CResource header (shared by all DB resources incl. SSpell) ---

struct CResource_vtable {
	int* add_ref; // 0x00 — FUN_008c61d0 path calls [vtable+0]
	int* release; // 0x08 — scalar deleting dtor; arg 1 on final release
	int* get_payload_size; // 0x10 — FUN_00849ff0 after lazy DB load
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	int* call_28;
	int* call_32;
	int* call_36;
};

// Refcounted DB object header @ SSpell+0x00
struct CResource_header {
	CResource_vtable* instance; // 0x00
	int ref_flags; // 0x04 — weak/strong flags (FUN_008c61d0)
	int ref_count; // 0x08 — strong refcount (FUN_00b8fdb0, FUN_0084da30)
	int field_0c; // 0x0C
	int field_10; // 0x10
	int field_14; // 0x14
	int field_18; // 0x18
	int field_1c; // 0x1C
	int name_begin; // 0x20 — blob/string begin (FUN_0084e640)
	int name_end; // 0x24
	int field_28; // 0x28
};

// Power/mastery table entry — 0x0C bytes each @ [SSpell+0x94..0x98] / [+0xA0..+0xA4]
// FUN_00a34c40: damage = mastery * entry[+8] + entry[+4]

struct SSpell_scaling_entry {
	float base; // 0x00
	float per_mastery; // 0x04
	float per_power; // 0x08
};

// FX timing vector entry — 8 bytes each @ [SSpell+0xDC..+0xE0]; func_GetSpellFxTiming

struct SSpell_fx_timing_entry {
	char data[8]; // +0x00 — timing float @ +0x60 in resolved sub-object (FUN_00c52ba0)
};

// NDb::SSpell POD layout (patch-relevant offsets only; full .xdb union is larger)

struct SSpell {
	CResource_header resource; // 0x00..0x2B
	char db_resolved; // 0x2C — lazy-load gate (FUN_0084da30)
	char pad_2d[0x57]; // 0x2D..0x83
	int tier; // 0x84 — func_GetSpellTier
	int school; // 0x88 — func_GetSpellSchool
	char pad_8c[0x90 - 0x8C];
	int mana_cost; // 0x90 — base mana (func_GetSpellManaCost)
	int mastery_table_begin; // 0x94
	int mastery_table_end; // 0x98
	int power_table_begin; // 0xA0
	int power_table_end; // 0xA4
	char pad_a8[0xCD - 0xA8];
	char field_cd; // 0xCD — FUN_00a34670
	char field_ce; // 0xCE — FUN_00a34650
	char pad_cf;
	int target_class; // 0xD0 — 0/1/2 (FUN_00a34500 / FUN_00a34520)
	int element; // 0xD4 — func_GetSpellElement / FUN_00a342f0
	char has_element; // 0xD8 — gate for func_GetSpellElement
	char pad_d9[0xDC - 0xD9];
	int fx_timing_begin; // 0xDC — vector begin
	int fx_timing_end; // 0xE0 — vector end
};

// Lazy-resolve handle passed to FUN_0084da30 (SpellDbRef)

struct SpellDbRef {
	int spell_ptr; // 0x00 — ref-counted SSpell* (may be 0)
	int flags; // 0x04 — low byte: resolved flag (FUN_0084da30)
};

// Combat spellRef — 5 dwords on stack (CombatHero +0x244, func_GetSpellDamage)

struct SpellRef {
	int spell_handle; // 0x00 — ref-counted handle (may be 0)
	int spell_id; // 0x04 — 0 = skip school bonuses in spellpower
	int field_08; // 0x08
	int field_0c; // 0x0C
	int field_10; // 0x10
};

// func_CopySpellCastContext / func_CastSpell context object

struct SpellCastContext {
	int spell_handle; // 0x00
	int spell_id; // 0x04
	int target_ids_begin; // 0x08 — vector<int> (FUN_00adfce0)
	int target_ids_end; // 0x0C
	int target_ids_cap; // 0x10
	int caster; // 0x14 — param_1[5] in func_CastSpell
	int target; // 0x18 — param_1[6] in func_CastSpell
	int field_1c; // 0x1C
	int field_20; // 0x20
	int field_24; // 0x24
	int field_28; // 0x28
	int field_2c; // 0x2C
	int field_30; // 0x30
	int field_34; // 0x34
	char flag_38; // 0x38 — param_1[0xE]
};

// --- Combat timeline events (allocated via func_EngineAlloc in func_CastSpell) ---

enum {
	CSpellFxEvent_size = 0x54,
	CSpellCastEvent_size = 0x60,
	CSpellDamageEvent_size = 0x60,
	CSpellSplashEvent_size = 0x64, // alloc 100 in one CastSpell path
	CSpellFxEvent_vtable = 0x00E8DC04,
	CSpellFxEvent_mi_vtable = 0x00E8DBE4,
	CSpellFxEvent_vtordisp = 0x44,
	CSpellCastEvent_vtable = 0x00E9207C,
	CSpellCastEvent_mi_vtable = 0x00E9205C,
	CSpellCastEvent_vtordisp = 0x50,
	CSpellSplashEvent_vtable = 0x00E925E8,
	CSpellSplashEvent_mi_vtable = 0x00E925C8,
	CSpellSplashEvent_vtordisp = 0x54,
};

struct CSpellFxEvent_vtable {
	int* call_0;
	// ... shared combat-event layout; primary vtable @ 0x00E8DC04
};

struct CSpellCastEvent_vtable {
	int* call_0;
	// ... primary @ 0x00E9207C; shares base with CSpellDamageEvent
};

struct CSpellSplashEvent_vtable {
	int* call_0;
	// ... primary @ 0x00E925E8
};

// --- Helpers ---

struct ISpellDb {
	SSpell* raw;

	explicit ISpellDb(SSpell* p = nullptr) : raw(p) {}

	int tier() const {
		return raw ? raw->tier : 0;
	}
	int school() const {
		return raw ? raw->school : SPELL_SCHOOL_UNKNOWN;
	}
	int mana_cost() const {
		return raw ? raw->mana_cost : 0;
	}
	int element() const {
		if (!raw || !raw->has_element) return SPELL_ELEMENT_NONE;
		return raw->element;
	}
	int target_class() const {
		return raw ? raw->target_class : 0;
	}
};

// Resolve effective spell id from a mass/empowered/slot id (func_GetSpellID logic).
inline int resolve_spell_id(int spell_id) {
	int mapped = 0;
	switch (spell_id) {
	case 0xD2: mapped = 0xB; break;
	case 0xD3: mapped = 0xD; break;
	case 0xD4: mapped = 0xC; break;
	case 0xD5: mapped = 0x11; break;
	case 0xD6: mapped = 0xE; break;
	case 0xD7: mapped = 0xF; break;
	case 0xD8: mapped = 0x17; break;
	case 0xD9: mapped = 0x1A; break;
	case 0xDA: mapped = 0x19; break;
	case 0xDB: mapped = 0x1D; break;
	case 0xDC: mapped = 0x1C; break;
	case 0xDD: mapped = 0x18; break;
	default: break;
	}
	if (mapped) return mapped;
	switch (spell_id) {
	case 0xDF: return 1;
	case 0xE0: return 2;
	case 0xE1: return 3;
	case 0xE2: return 4;
	case 0xE3: return 5;
	case 0xE4: return 6;
	case 0xE5: return 7;
	case 0xE6: return 8;
	case 0xE7: return 9;
	case 0xE8: return 10;
	case 0xE9: return 0xED;
	case 0x12A: return 0x117;
	default: return spell_id;
	}
}

#endif
