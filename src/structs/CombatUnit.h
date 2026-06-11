#pragma once
#ifndef COMBAT_UNIT_H
#define COMBAT_UNIT_H

// Shared MSVC virtual interface for in-combat fighters (NWorld::CCombatHero, NWorld::CCombatCreature).
// Parallel vtables with identical slot layout (offsets 0..0x2A8); 92/171 slots resolve to the same impl.
// RE rule: .cursor/rules/h5x-re-combat-unit.mdc (auto-synced).
//
// Vtable instances:
//   CCombatHero     @ 0x00E8499C, vtordisp -0x118 (Ghidra: struct_CombatHero in func_InitCombat)
//   CCombatCreature @ 0x00E4FBA4, vtordisp -0x144 (inherits CTimedStats@NWorld per RTTI)
//
// Thunks: sub ecx,[ecx-4]; sub ecx,<vtordisp>; jmp <impl>. Hero vtordisp 0x68, creature 0x94.

typedef int (__thiscall* GetStat)(int*);
typedef int (__thiscall* GetSkillMastery)(int*, int);
typedef int (__thiscall* HasHeroSpec)(int*, int);
typedef int* (__thiscall* GetUnitRef)(int*);
typedef int (__thiscall* GetActiveBuffById)(int*, int);
typedef int (__thiscall* HasCreatureAbility)(int*, int);
typedef float (__thiscall* GetStatFloat)(int*);
typedef int* (__thiscall* GetATBInfo)(int*);
typedef void (__thiscall* SetATBValue)(int*, float);
typedef int (__thiscall* GetCombatSpellpower)(int*, int*, int);

enum {
	CombatHero_vtable_addr = 0x00E8499C,
	CombatCreature_vtable_addr = 0x00E4FBA4,
	CombatHero_vtordisp = 0x118,
	CombatCreature_vtordisp = 0x144
};

struct CombatUnit_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	GetUnitRef get_unit_ref; // 0x0C (12) - 0x008AEAF0 [shared], returns [this-0x74]
	int* call_16;
	int* call_20;
	int* call_24;
	GetActiveBuffById get_active_buff; // 0x28 (40) - 0x00A6D330 [shared]
	int* call_32;
	int* call_36; // 0x24 - hero: forwards to IHero +0x284
	int* call_40;
	int* call_44;
	int* call_48;
	int* call_52; // 0x34 - hero: forwards to IHero +0x268
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
	int* call_264;
	int* call_268;
	int* call_272;
	int* call_276;
	int* call_280;
	int* call_284;
	int* call_288;
	int* call_292;
	int* call_296;
	int* call_300;
	int* call_304;
	int* call_308;
	int* call_312;
	int* call_316;
	int* call_320;
	int* call_324;
	int* call_328;
	int* call_332;
	int* call_336;
	int* call_340;
	int* call_344;
	int* call_348;
	int* call_352;
	int* call_356;
	int* call_360;
	int* call_364;
	int* call_368;
	int* call_372;
	int* call_376;
	int* call_380;
	int* call_384;
	int* call_388; // 0x184 - same implementation as get_atb_info
	GetATBInfo get_atb_info; // 0x188 (392) - 0x008AEB40 [shared], ATB float at [info+0x1C]
	SetATBValue set_atb; // 0x18C (396) - 0x00A6B690 [shared] func_SetUnitATB
	int* call_400;
	int* call_404;
	GetStat get_attack; // 0x198 (408) - hero 0x00BBFE50, creature 0x008A4800
	GetStat get_defense; // 0x19C (412) - hero 0x00BBFE60, creature 0x008A32B0
	int* call_416;
	int* call_420;
	int* call_424;
	GetStat get_luck; // 0x1AC (428) - 0x00A6BD10 [shared]
	GetStat get_morale; // 0x1B0 (432) - 0x00A6BD50 [shared]
	GetStat get_luck_raw; // 0x1B4 (436) - hero 0x00BBFF00, creature 0x00A6ABF0
	GetStat get_morale_raw; // 0x1B8 (440) - hero 0x00BBFFC0, creature 0x00A6ACD0
	GetStatFloat get_initiative; // 0x1BC (444) - hero 0x00BC1EB0, creature 0x008A4330
	int* call_448;
	int* call_452;
	int* call_456;
	int* call_460;
	int* call_464;
	int* call_468;
	int* call_472;
	int* call_476;
	int* call_480;
	int* call_484;
	int* call_488;
	int* call_492;
	int* call_496;
	int* call_500;
	int* call_504;
	int* call_508;
	int* call_512;
	int* call_516;
	int* call_520;
	int* call_524;
	int* call_528;
	int* call_532;
	int* call_536;
	int* call_540;
	int* call_544;
	int* call_548;
	int* call_552;
	int* call_556;
	int* call_560;
	int* call_564;
	GetStat get_mana; // 0x238 (568) - hero 0x00BC0700; creature 0x008A2230 (mana points)
	GetStat get_hero_level; // 0x23C (572) - hero 0x00BBFB90; creature: unused slot
	GetCombatSpellpower get_spell_mastery; // 0x240 (576) - hero 0x00A6A700; creature: unused
	GetCombatSpellpower get_spellpower; // 0x244 (580) - hero 0x00A6A7E0; creature: unused
	int* call_584;
	int* call_588;
	int* call_592;
	int* call_596;
	int* call_600;
	int* call_604;
	int* call_608;
	int* call_612;
	int* call_616;
	int* call_620;
	int* call_624;
	int* call_628;
	int* call_632;
	int* call_636;
	int* call_640;
	int* call_644;
	int* call_648;
	HasCreatureAbility has_ability; // 0x28C (652) - creature 0x008A34C0; hero: call_652
	GetSkillMastery has_skill; // 0x290 (656) - hero 0x00BBFED0; creature stub 0x00A332C0
	HasHeroSpec has_hero_spec; // 0x294 (660) - hero 0x00BBFEF0; creature stub 0x00A332C0
	int* call_664;
	int* call_668;
	int* call_672;
	int* call_676;
	int* call_680;
};

using CombatHero_vtable = CombatUnit_vtable;
using CombatCreature_vtable = CombatUnit_vtable;

struct ICombatUnit {
	CombatUnit_vtable* instance;

	int* unit_ref() {
		return instance->get_unit_ref((int*)this);
	}
	int* atb_info() {
		return instance->get_atb_info((int*)this);
	}
	float atb() {
		return *(float*)(atb_info() + 0x1C / 4);
	}
	void set_atb(float value) {
		instance->set_atb((int*)this, value);
	}
	int attack() {
		return instance->get_attack((int*)this);
	}
	int defense() {
		return instance->get_defense((int*)this);
	}
	int luck() {
		return instance->get_luck((int*)this);
	}
	int morale() {
		return instance->get_morale((int*)this);
	}
	float initiative() {
		return instance->get_initiative((int*)this);
	}
	int active_buff(int ability_id) {
		return instance->get_active_buff((int*)this, ability_id);
	}
};

#endif
