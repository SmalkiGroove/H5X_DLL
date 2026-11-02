#pragma once
#ifndef COMBAT_H
#define COMBAT_H

// Combat objects other than the units (CombatUnit.h, Hero.h, Creature.h): the combat, its arena, generator,
// unit names and side armies. RE rules: .cursor/rules/h5x-re-combat-atb.mdc, .cursor/rules/h5x-re-lua-engine.mdc.
#include "types/types.h"
#include "structs/AdvMap.h"

typedef int(__thiscall* RandomRange)(int*, int, int);
typedef void(__thiscall* GetGridSize)(int*, int*, int*);
typedef int*(__thiscall* GetArmy)(int*, int);
typedef int*(__thiscall* FindUnit)(int*, const char**);
typedef char**(__thiscall* GetUnitName)(int*, int*);

// NWorld::IRandomGenerator (NWorld::CRandomGenerator subobject +0x1C, vtable 0x00E51284). LCG (0x343FD, 0x269EC3)
// over the state at +0x08.
enum {
	RandomGenerator_vtable_addr = 0x00E51284
};

struct RandomGenerator_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	RandomRange random_range; // 0x0C (12) - 0x008C04C0, thiscall(min, max) -> [min, max] through +0x10
	CheckValue random; // 0x10 (16) - 0x008C0470, thiscall(n) -> [0, n), advances the state
};

struct IRandomGenerator {
	RandomGenerator_vtable* instance;
	int pad_04;
	unsigned int state; // +0x08

	int random(int range) {
		return instance->random((int*)this, range);
	}
	int random_range(int min, int max) {
		return instance->random_range((int*)this, min, max);
	}
};

// Script names of the combat units (CCombat +0x140, from Combat_vtable get_unit_names). Concrete vtable not identified.
struct CombatUnitNames_vtable {
	int* call_0;
	FindUnit find_unit; // 0x04 (4) - thiscall(engine string* name) -> CombatUnit, nullptr when unknown
	GetUnitName get_name; // 0x08 (8) - thiscall(CombatUnit*) -> engine string* {begin, end}
};

struct ICombatUnitNames {
	CombatUnitNames_vtable* instance;

	// name: engine string {begin, end, capacity end}, may point to a C string (not freed)
	int* find_unit(const char** name) {
		return instance->find_unit((int*)this, name);
	}
	char** name(int* unit) {
		return instance->get_name((int*)this, unit);
	}
};

// NWorld::CCombat (vtable 0x00E79D3C, primary), from Arena_vtable get_combat or ICombatUnit::combat().
enum {
	Combat_vtable_addr = 0x00E79D3C
};

struct Combat_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	GetPtr get_random; // 0x0C (12) - 0x00AF80F0, returns [this+0xD4]: IRandomGenerator of the battle (luck, morale, abilities)
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
	GetPtr get_week; // 0x3C (60) - 0x00620240, returns [this+0xA0]: week of the element bonus, passed to prepare_spell_damage
	int* call_64;
	GetGridSize get_grid_size; // 0x44 (68) - 0x00AF1450, thiscall(int* width, int* height): battlefield size in cells
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
	GetArmy get_army; // 0x84 (132) - 0x00AF2140, thiscall(1 for side 0 (attacker), 0 for side 1) -> ICombatArmy (CombatUnit.h), as lua_GetHost (0x00567000)
	int* call_136;
	int* call_140;
	int* call_144;
	int* call_148;
	int* call_152;
	int* call_156;
	int* call_160;
	int* call_164;
	int* call_168;
	int* call_172;
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
	int* call_220;
	int* call_224;
	int* call_228;
	int* call_232;
	int* call_236;
	int* call_240;
	int* call_244;
	int* call_248;
	int* call_252;
	int* call_256;
	int* call_260;
	GetFlag is_silent; // 0x108 (264) - 0x00AF7F00, returns [this+0x98]: silent / estimate combat (no combat events)
	int* call_268;
	int* call_272;
	int* call_276;
	int* call_280;
	int* call_284;
	GetPtr get_unit_names; // 0x120 (288) - 0x00AF8160, returns [this+0x140]: ICombatUnitNames
};

struct ICombat {
	Combat_vtable* instance;

	IRandomGenerator* random_generator() {
		return (IRandomGenerator*)instance->get_random((int*)this);
	}
	int* week() {
		return instance->get_week((int*)this);
	}
	void grid_size(int* width, int* height) {
		instance->get_grid_size((int*)this, width, height);
	}
	// side: 0 attacker, 1 defender
	ICombatArmy* army(int side) {
		return (ICombatArmy*)instance->get_army((int*)this, side == 0 ? 1 : 0);
	}
	bool is_silent() {
		return instance->is_silent((int*)this);
	}
	ICombatUnitNames* unit_names() {
		return (ICombatUnitNames*)instance->get_unit_names((int*)this);
	}
};

// NCombatArenaScript::ICombatArenaCommon: owner of the "combat" Lua engine, a virtual base of NUI::CCombatArenaScreen
// (subobject +0x57C, vtable 0x00E1D95C).
enum {
	Arena_vtable_addr = 0x00E1D95C
};

struct Arena_vtable {
	int* call_0;
	PostCommand post_command; // 0x04 (4) - 0x005528F0 -> 0x0053E110, thiscall(command): forwards to [this+0xC] vtable +0x00
	GetPtr get_combat; // 0x08 (8) - 0x00552900 -> 0x0053E140: ICombat
	FindUnit get_unit_by_name; // 0x0C (12) - 0x005527A0 -> 0x00565CA0: get_combat, get_unit_names, find_unit
};

struct IArena {
	Arena_vtable* instance;

	void post_command(int* command) {
		instance->post_command((int*)this, command);
	}
	ICombat* combat() {
		return (ICombat*)instance->get_combat((int*)this);
	}
};

#endif
