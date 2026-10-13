#include "pch.h"
#include "Artfset.h"

// OFFENSIVE AND DEFENSIVE FORMATION APPLY TO ANY CREATURE (vanilla: Fortress creatures only)
// The bonus counts once per source: the perk, and 4 pieces of the Dwarven set (ArtfsetDwarven.cpp); both = x2.
// Vanilla bonus: min(trunc(log10(adjacent creatures / stack count) * k), DefaultStats +0x6B0 +0x1C (attack) / +0x20 (defense)), >= 0.

void OffensiveFormationCheckFork();
void OffensiveFormationBonusFork();
void DefensiveFormationCheckFork();
void DefensiveFormationBonusFork();

// func_GetUnitAttack (0x008A4800), edi = hero (outer, not null)
int OffensiveFormationCheck_fork = 0x008A4A0A;    // push 0A0h / call has_skill / test al,al / je
int OffensiveFormationCheck_apply = 0x008A4A19;
int OffensiveFormation_skip = 0x008A4A7D;
int OffensiveFormationBonus_fork = 0x008A4A6F;    // eax = capped bonus: xor ecx,ecx / test / setl / sub / and (max 0)
int OffensiveFormationBonus_return = 0x008A4A7B;  // add ebx,eax
// func_GetUnitDefense (0x008A22F0), ebp = hero (outer, not null)
int DefensiveFormationCheck_fork = 0x008A24FF;    // hero vbase adjust / push 0A1h / call has_skill / test al,al / je
int DefensiveFormationCheck_apply = 0x008A251E;
int DefensiveFormation_skip = 0x008A2584;
int DefensiveFormationBonus_fork = 0x008A2576;    // same as the attack
int DefensiveFormationBonus_return = 0x008A2582;  // add ebx,eax

void OffDefFormation_init(pugi::xml_document& doc) {
	// Fortress-only checks (creature town == 9)
	assembly_patches.push_back({ PATCH_NOP, 0x008A24F1, 6, nullptr, 0, 0, 0, 0 });   // defense
	assembly_patches.push_back({ PATCH_NOP, 0x008A49EE, 6, nullptr, 0, 0, 0, 0 });   // attack
	assembly_patches.push_back({ PATCH_NOP, 0x008A296E, 6, nullptr, 0, 0, 0, 0 });   // adjacent creature count: own stack
	assembly_patches.push_back({ PATCH_NOP, 0x008A29E6, 2, nullptr, 0, 0, 0, 0 });   // adjacent creature count: neighbours
	assembly_patches.push_back({ PATCH_HOOK, OffensiveFormationCheck_fork, 15, OffensiveFormationCheckFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, OffensiveFormationBonus_fork, 12, OffensiveFormationBonusFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DefensiveFormationCheck_fork, 31, DefensiveFormationCheckFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DefensiveFormationBonus_fork, 12, DefensiveFormationBonusFork, 0, 0, 0 });
}

// Number of sources of the formation bonus for the hero (outer pointer, vbptr at +4).
static int formation_sources(int* hero_outer, int perk) {
	ICombatHero* hero = combat_creature_detail::combat_hero_from_outer(hero_outer);
	if (!hero) return 0;
	return (hero->has_perk(perk) ? 1 : 0) + artfset_dwarven_formation(hero->hero());
}

static int __fastcall offensive_formation_sources(int* hero_outer) {
	return formation_sources(hero_outer, PERK_OFFENSIVE_FORMATION);
}

static int __fastcall defensive_formation_sources(int* hero_outer) {
	return formation_sources(hero_outer, PERK_DEFENSIVE_FORMATION);
}

static int __fastcall offensive_formation_bonus(int* hero_outer, int bonus) {
	return bonus > 0 ? bonus * offensive_formation_sources(hero_outer) : 0;
}

static int __fastcall defensive_formation_bonus(int* hero_outer, int bonus) {
	return bonus > 0 ? bonus * defensive_formation_sources(hero_outer) : 0;
}

__declspec(naked) void OffensiveFormationCheckFork() {
	__asm
	{
		mov ecx, edi
		call offensive_formation_sources
		test eax, eax
		jz OFFENSIVE_FORMATION_SKIP
		jmp [OffensiveFormationCheck_apply]

	OFFENSIVE_FORMATION_SKIP:
		jmp [OffensiveFormation_skip]
	}
}

__declspec(naked) void OffensiveFormationBonusFork() {
	__asm
	{
		mov edx, eax
		mov ecx, edi
		call offensive_formation_bonus
		jmp [OffensiveFormationBonus_return]
	}
}

// Keeps ebp = hero (the vanilla code turned it into the adjusted interface)
__declspec(naked) void DefensiveFormationCheckFork() {
	__asm
	{
		mov ecx, ebp
		call defensive_formation_sources
		test eax, eax
		jz DEFENSIVE_FORMATION_SKIP
		jmp [DefensiveFormationCheck_apply]

	DEFENSIVE_FORMATION_SKIP:
		jmp [DefensiveFormation_skip]
	}
}

__declspec(naked) void DefensiveFormationBonusFork() {
	__asm
	{
		mov edx, eax
		mov ecx, ebp
		call defensive_formation_bonus
		jmp [DefensiveFormationBonus_return]
	}
}
