#pragma once
#ifndef HERO_H
#define HERO_H

// AdvMap hero interface (vtable 0x00E7E484, NWorld::CHero subobject +0x1C).
// RE rule: .cursor/rules/h5x-re-hero-stats.mdc (auto-synced).
#include "structs/CombatUnit.h"
#include "types/Functions.h"

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
	GetAdvMapShared get_advmap_shared; // 0x250 (592) - 0x00B531E0 thunk → lazy-load SAdvMapHeroShared @ [this+0xD0]
	int* call_596;
	GetStat get_hero_class; // 0x258 (600)
	int* call_604;
	int* call_608;
	int* call_612;
	GetNameWString get_name_wstring; // 0x268 (616) - 0x00B4D4A0, copies wchar range [this+0x6C..0x70] into out-vector
	int* call_620;
	int* call_624;
	int* call_628;
	int* call_632;
	int* call_636;
	int* call_640;
	GetPortraitTexture get_portrait_texture; // 0x284 (644) - 0x00B532F0, lazy-load STexture via SAdvMapHeroShared
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
	int* name_wstring(int* out) {
		return instance->get_name_wstring((int*)this, out);
	}
	int portrait_texture() {
		return instance->get_portrait_texture((int*)this);
	}
};

// CombatHero (vtable 0x00E8499C) — extends shared CombatUnit interface.
// Subobject: mov edx,[edi+4] / mov eax,[edx+8] / lea ecx,[eax+edi+4]
// AdvMap hero (IHero) at [this - 0x6C].

struct ICombatHero : ICombatUnit {
	IHero* hero() {
		return *(IHero**)((int*)this - 0x6C / 4);
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
	bool has_artifact(int artifact_id) {
		IHero* advHero = hero();
		if (!advHero || !advHero->instance) return false;
		int* inventory = advHero->inventory();
		if (!inventory) return false;
		return count_equipped_artifact(inventory, artifact_id) != 0;
	}
};

#endif