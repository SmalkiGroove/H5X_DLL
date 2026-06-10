#pragma once
#ifndef HERO_H
#define HERO_H

// AdvMap hero interface (vtable 0x00E7E484, NWorld::CHero subobject +0x1C).
// See docs/RE_hero_stats.md for the reverse-engineering details.
typedef int* (__thiscall* GetInventory)(int*);
typedef int (__thiscall* GetSkillMastery)(int*, int);
typedef int (__thiscall* GetStat)(int*);
typedef int (__thiscall* GetMaxMana)(int*, int);
typedef int (__thiscall* GetSpellpowerForSpell)(int*, int*);
typedef int (__thiscall* HasHeroSpec)(int*, int);

struct Hero_vtable {
	int* call_0;
	int* call_4;
	GetStat get_luck_raw; // 0x08 (8) - 0x00B4CA10, unclamped, base + has_skill(SKILL_LUCK) bonus
	GetStat get_morale_raw; // 0x0C (12) - 0x00B4CAE0, unclamped, base + has_skill(SKILL_LEADERSHIP) bonus
	GetStat get_attack; // 0x10 (16) - 0x00B4CC50, total, clamped >= 0
	GetStat get_defense; // 0x14 (20) - 0x00B4CCC0, total
	GetStat get_spellpower; // 0x18 (24) - 0x00B4CCF0, total
	GetStat get_knowledge; // 0x1C (28) - 0x00B4CD80, total, clamped >= 1
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
	GetInventory get_inventory; // 0x74 (116)
	GetInventory get_backpack; // 0x78 (120)
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
	GetStat get_mana_points; // 0x12C (300) - 0x00AF8160, returns [this+0x140]
	GetMaxMana get_max_mana; // 0x130 (304) - 0x00B4C8F0, arg = knowledge, applies Intelligence bonus
	int* call_308;
	int* call_312;
	GetStat get_luck; // 0x13C (316) - 0x00B4CB40, get_luck_raw clamped to [-5, +5]
	GetStat get_morale; // 0x140 (320) - 0x00B4CB60, get_morale_raw clamped to [-5, +5]
	int* call_324;
	int* call_328;
	int* call_332;
	int* call_336;
	int* call_340;
	int* call_344;
	int* call_348;
	int* call_352;
	GetSpellpowerForSpell get_spellpower_for_spell; // 0x164 (356) - 0x00B4B3E0, arg = spellRef, returns 8 for spellpower-independent spells
	int* call_360;
	int* call_364;
	int* call_368;
	GetSkillMastery get_skill_mastery; // 0x174 (372) - 0x00B4D150, also used as has_skill
	int* call_376;
	int* call_380;
	int* call_384;
	int* call_388;
	int* call_392;
	int* is_perk_learnable; // 0x18C (396)
	HasHeroSpec has_hero_spec; // 0x190 (400) - 0x00B4B5A0, compares arg to [this+0x8C]
	GetStat get_hero_spec; // 0x194 (404) - 0x00B4B5C0, returns [this+0x8C]
	int* call_408;
	int* call_412;
	int* call_416;
	int* call_420;
	int* call_424;
	int* call_428;
	int* call_432;
	int* call_436;
	int* has_free_skill_slot;
	int* call_444;
	int* call_448;
	int* is_skill_at_max_mastery;
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
	int* call_568;
	int* call_572;
	int* call_576;
	int* call_580;
	int* call_584;
	int* call_588;
	int* call_592;
	int* call_596;
	GetStat get_hero_class; // 0x258 (600)
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
	int* call_652;
	int* call_656;
	int* call_660;
	int* call_664;
	int* call_668;
	int* call_672;
	int* call_676;
	int* call_680;
	int* call_684;
	int* call_688;
	int* call_692;
	int* call_696;
	int* call_700;
};

struct IHero {
	Hero_vtable* instance;

	int* inventory() {
		return instance->get_inventory((int*)this);
	}
	int* backpack() {
		return instance->get_backpack((int*)this);
	}
	int attack() {
		return instance->get_attack((int*)this);
	}
	int defense() {
		return instance->get_defense((int*)this);
	}
	int spellpower() {
		return instance->get_spellpower((int*)this);
	}
	int knowledge() {
		return instance->get_knowledge((int*)this);
	}
	int luck() {
		return instance->get_luck((int*)this);
	}
	int morale() {
		return instance->get_morale((int*)this);
	}
	int mana_points() {
		return instance->get_mana_points((int*)this);
	}
	int hero_class() {
		return instance->get_hero_class((int*)this);
	}
	int hero_spec() {
		return instance->get_hero_spec((int*)this);
	}
	bool has_spec(int spec) {
		return instance->has_hero_spec((int*)this, spec) != 0;
	}
	int skill_mastery(int skill) {
		return instance->get_skill_mastery((int*)this, skill);
	}
};

// CombatHero interface (vtable 0x00E8499C, NWorld::CCombatHero subobject).
// See docs/RE_hero_stats.md for the reverse-engineering details.
// All entries go through vtordisp thunks, so calling through the vtable handles
// the this-pointer adjustment automatically. The subobject pointer is obtained
// with the usual vbtable pattern, e.g. from a unit owner in edi:
//   mov edx, [edi + 0x4] / mov eax, [edx + 0x8] / lea ecx, [eax + edi + 0x4]
// The AdvMap hero interface (IHero) pointer is stored at [this - 0x6C].

typedef float (__thiscall* GetStatFloat)(int*);
typedef int* (__thiscall* GetATBInfo)(int*);
typedef void (__thiscall* SetATBValue)(int*, float);
typedef int (__thiscall* GetCombatSpellpower)(int*, int*, int);

struct CombatHero_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	int* call_28;
	int* call_32;
	int* call_36; // 0x24 - forwards to IHero +0x284
	int* call_40;
	int* call_44;
	int* call_48;
	int* call_52; // 0x34 - forwards to IHero +0x268
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
	GetATBInfo get_atb_info; // 0x188 (392) - 0x008AEB40, returns [this-0x70], current ATB float at +0x1C of the result
	SetATBValue set_atb; // 0x18C (396) - 0x00A6B690, value in [0..1]
	int* call_400;
	int* call_404;
	GetStat get_attack; // 0x198 (408) - 0x00BBFE50, IHero attack + combat bonus [this+0x3C]
	GetStat get_defense; // 0x19C (412) - 0x00BBFE60, IHero defense + combat bonus [this+0x40]
	int* call_416;
	int* call_420;
	int* call_424;
	GetStat get_luck; // 0x1AC (428) - 0x00A6BD10, get_luck_raw clamped to [-5, +5]
	GetStat get_morale; // 0x1B0 (432) - 0x00A6BD50, get_morale_raw clamped to [-5, +5]
	GetStat get_luck_raw; // 0x1B4 (436) - 0x00BBFF00, IHero raw luck + combat modifiers
	GetStat get_morale_raw; // 0x1B8 (440) - 0x00BBFFC0, IHero raw morale + combat modifiers
	GetStatFloat get_initiative; // 0x1BC (444) - 0x00BC1EB0, hooked by ModInitiative
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
	GetStat get_mana; // 0x238 (568) - 0x00BC0700, IHero get_max_mana(knowledge) + get_mana_points()
	GetStat get_hero_level; // 0x23C (572) - 0x00BBFB90, IHero level + combat bonus
	GetCombatSpellpower get_spell_mastery; // 0x240 (576) - 0x00A6A700, args = (spellRef*, flag)
	GetCombatSpellpower get_spellpower; // 0x244 (580) - 0x00A6A7E0, effective spellpower incl. combat buffs, args = (spellRef*, flag 0)
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
	int* call_652;
	GetSkillMastery has_skill; // 0x290 (656) - 0x00BBFED0, forwards to IHero get_skill_mastery (+0x174)
	HasHeroSpec has_hero_spec; // 0x294 (660) - 0x00BBFEF0, forwards to IHero has_hero_spec (+0x190)
	int* call_664;
	int* call_668;
	int* call_672;
	int* call_676;
	int* call_680;
};

struct ICombatHero {
	CombatHero_vtable* instance;

	IHero* hero() {
		return *(IHero**)((int*)this - 0x6C / 4);
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
	int mana() {
		return instance->get_mana((int*)this);
	}
	int hero_level() {
		return instance->get_hero_level((int*)this);
	}
	// Effective spellpower with no spell context (skills + artifacts + combat buffs)
	int spellpower() {
		int spellRef[5] = { 0, 0, 0, 0, 0 };
		return instance->get_spellpower((int*)this, spellRef, 0);
	}
	int spellpower(int* spellRef) {
		return instance->get_spellpower((int*)this, spellRef, 0);
	}
	int spell_mastery(int* spellRef) {
		return instance->get_spell_mastery((int*)this, spellRef, 0);
	}
	int has_skill(int skill) {
		return instance->has_skill((int*)this, skill);
	}
	bool has_spec(int spec) {
		return instance->has_hero_spec((int*)this, spec) != 0;
	}
};

#endif