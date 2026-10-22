#include "pch.h"

// Skill Thrill of Combat : the hero can get good morale after attacking, like creatures
// Creatures roll good morale (CombatUnit +0x1CC) at the end of each action and on success run the good morale effect
// (+0x164, 0x00A6CF90: GOOD_MORALE message, ATB set to DefaultStats +0xAD4) instead of resetting their ATB (attack end
// 0x00A65B5C). The hero +0x1CC returns false, and the hero attack resets the ATB without rolling:
// func_ComputeHeroAttackEffects calls func_HeroAttackATBReset (0x008CE8E0) at 0x008CFB27, edi = time returned by
// func_HeroDefaultAttack. After that call, a hero with the skill runs the creature roll (0x00A6BA00: morale > 0, chance
// morale * 10%) and on success gets the effect; the ATB set by the attack reset is kept when it is higher.
// Spells (+0x224, 0x00BBF610) and func_CheckUnitEffect (case 0x3D also calls func_HeroAttackATBReset, 0x00984E60) don't
// roll.

void ThrillOfCombatAttackEnd();

int ThrillOfCombat_roll = 0x00A6BA00;            // good morale roll (creature +0x1CC impl)
int ThrillOfCombat_attack_atb = 0x008CE8E0;      // func_HeroAttackATBReset
int ThrillOfCombatAttack_call = 0x008CFB27;      // func_ComputeHeroAttackEffects: call func_HeroAttackATBReset

void ThrillOfCombat_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_CALL, ThrillOfCombatAttack_call, 5, ThrillOfCombatAttackEnd, 0, 0, 0 });
}

typedef char(__thiscall* RollGoodMoraleFn)(int*);
typedef int(__thiscall* GoodMoraleEffectFn)(int*, int);

// outer = NWorld::ICombatHero interface (func_HeroAttackATBReset `this`), time = end of the attack
static void __stdcall thrill_of_combat_attack_end(int* outer, int time) {
	ICombatHero* hero = combat_creature_detail::combat_hero_from_outer(outer);
	if (!hero || !hero->has_perk(SPEC_THRILL_OF_COMBAT)) return;
	// `this` of the roll: same adjustment as the hero thunks of the shared impls (+0x164: 0x00BC2E10)
	int* roll_this = (int*)((char*)hero - *((int*)hero - 1) - 0x68);
	if (!((RollGoodMoraleFn)ThrillOfCombat_roll)(roll_this)) return;
	float atb = hero->atb();
	((GoodMoraleEffectFn)hero->instance->schedule_timeline_hook_164)((int*)hero, time);
	if (atb > hero->atb())
		hero->set_atb(atb);
}

// Replaces `call func_HeroAttackATBReset`: ecx = NWorld::ICombatHero interface, edi = time. esi / edi are kept.
__declspec(naked) void ThrillOfCombatAttackEnd() {
	__asm
	{
		push ecx
		call [ThrillOfCombat_attack_atb]
		pop ecx
		push edi
		push ecx
		call thrill_of_combat_attack_end
		ret
	}
}
