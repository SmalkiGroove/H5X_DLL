#include "pch.h"

// ARTIFICIER: bonuses of the creature mini-artifacts (Academy skill Artificier), scaling on the hero's Knowledge (K).
// Replaces func_ComputeArtificierItemBonus (0x00C0A2F0, fastcall(effect, knowledge) -> bonus, -1 for an unknown effect),
// called when a mini-artifact is crafted (0x00C0A922) and when the hero's Knowledge rises (0x00C0A795, bonuses only go up).
//
// Two kinds of formulas:
// - curve: 1 at 1 Knowledge up to `max` at 100 Knowledge, with diminishing returns (about half the maximum at 25 Knowledge,
//   three quarters at 50): 1 + (max - 1) * k * (99 + 50) / (99 * (k + 50)), k = min(K, 100) - 1
// - linear: min(K / divisor + 1, max)
//
// Effect (MicroArtifactEffects.xdb), how the game applies it, vanilla formula -> new formula:
//   1  Wounding           no combat effect found in H5X.exe                   K/5 + 1         (kept)
//   2  Piercing           + attack (func_GetCreatureAttack)                   K/4 + 1         -> curve, max 16
//   3  Armor Crushing     - target defense on hit (buff 0xB3, FUN_00a629b0)   K/15 + 1        -> K/3 + 1, max 20
//   4  Health             + hit points (func_GetCreatureHealth)               K/5 + 1 (flat)  -> K/4 + 1, max 20 (% health)
//   5  Defence            + defense (func_GetCreatureDefense)                 K/4 + 1         -> curve, max 16
//   6  Magic Protection   - % spell damage (func_ComputeSpellDamageMitigation) 85% from K=60  -> curve, max 50
//   7  Luck               + luck                                              K/10 + 1        -> curve, max 3
//   8  Morale             + morale                                            K/10 + 1        -> curve, max 3
//   9  Speed              + speed (func_GetCreatureSpeed)                     K/15 + 1        -> curve, max 2
//   10 Haste              + % initiative, max 50 (func_GetCreatureInitiative) K               -> K/4 + 1, max 20
// Bonuses are stored as signed bytes (max 127).
//
// Health is changed from flat to percent in func_GetCreatureHealth (0x004BC470): the flat add is removed, and the
// percent is added to the hero's health percent (IHero vtable +0x330, +0x10), or applied alone without hero.

enum MicroArtifactEffect {
	MAE_NONE = 0,
	MAE_WOUNDING = 1,
	MAE_PIERCING = 2,
	MAE_ARMOR_CRUSHING = 3,
	MAE_HEALTH = 4,
	MAE_DEFENCE = 5,
	MAE_MAGIC_PROTECTION = 6,
	MAE_LUCK = 7,
	MAE_MORALE = 8,
	MAE_SPEED = 9,
	MAE_HASTE = 10,
};

int __fastcall artificier_item_bonus(int effect, int knowledge);
void ArtificierHealthHeroFork();
void ArtificierHealthNoHeroFork();

int Artificier_fork = 0x00C0A2F0;                // func_ComputeArtificierItemBonus entry: sub esp,0Ch / test edx,edx
int ArtificierHealthFlat_site = 0x004BC4C1;      // movsx eax,byte ptr [esp+10h] / add edi,eax (mini-artifact flat health)
int ArtificierHealthHero_fork = 0x004BC5E7;      // mov eax,[eax+10h] / add eax,64h (hero health percent + 100)
int ArtificierHealthHero_return = 0x004BC5ED;
int ArtificierHealthNoHero_fork = 0x004BC624;    // no hero: mov eax,edi / pop edi / pop esi / pop ebx
int ArtificierHealthNoHero_return = 0x004BC629;  // add esp,10h / ret 8

const int artificier_max_knowledge = 100;
const double artificier_curve = 50.0;  // bend of the curve: lower = stronger early, higher = closer to linear

void Artificier_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Artificier_fork, 5, (void*)artificier_item_bonus, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, ArtificierHealthFlat_site, 7, nullptr, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtificierHealthHero_fork, 6, ArtificierHealthHeroFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtificierHealthNoHero_fork, 5, ArtificierHealthNoHeroFork, 0, 0, 0 });
}

static int curve_bonus(int knowledge, int max) {
	if (knowledge > artificier_max_knowledge) knowledge = artificier_max_knowledge;
	double k = knowledge - 1;
	double span = artificier_max_knowledge - 1;
	double ratio = k * (span + artificier_curve) / (span * (k + artificier_curve));  // 0 at 1 Knowledge, 1 at 100
	return (int)(1 + (max - 1) * ratio + 0.5);
}

static int linear_bonus(int knowledge, int divisor, int max) {
	int bonus = knowledge / divisor + 1;
	return bonus < max ? bonus : max;
}

// Same calling convention as the replaced function: the hook jumps here with the caller's return address on the stack.
int __fastcall artificier_item_bonus(int effect, int knowledge) {
	if (knowledge < 1) knowledge = 1;
	switch (effect) {
	case MAE_WOUNDING:          return knowledge / 5 + 1;  // vanilla
	case MAE_PIERCING:          return curve_bonus(knowledge, 16);
	case MAE_ARMOR_CRUSHING:    return linear_bonus(knowledge, 3, 20);
	case MAE_HEALTH:            return linear_bonus(knowledge, 4, 20);
	case MAE_DEFENCE:           return curve_bonus(knowledge, 16);
	case MAE_MAGIC_PROTECTION:  return curve_bonus(knowledge, 50);
	case MAE_LUCK:              return curve_bonus(knowledge, 3);
	case MAE_MORALE:            return curve_bonus(knowledge, 3);
	case MAE_SPEED:             return curve_bonus(knowledge, 2);
	case MAE_HASTE:             return linear_bonus(knowledge, 4, 20);
	}
	return -1;
}

// With hero: eax = hero health bonuses (+0x10 = percent), ebp = health. The mini-artifact stats were copied at [esp+0xC]
// (health byte at +4: [esp+0x10] in the body, [esp+0x14] after push ebp). Next: clamp to [0, 1000], health * eax / 100.
__declspec(naked) void ArtificierHealthHeroFork() {
	__asm
	{
		mov eax, dword ptr [eax + 0x10]
		movsx ecx, byte ptr [esp + 0x14]
		add eax, ecx
		add eax, 0x64
		jmp [ArtificierHealthHero_return]
	}
}

// Without hero: edi = health, mini-artifact health percent at [esp+0x10]. Same rounding as the hero path.
__declspec(naked) void ArtificierHealthNoHeroFork() {
	__asm
	{
		movsx eax, byte ptr [esp + 0x10]
		add eax, 0x64
		imul eax, edi
		add eax, 0x32
		cdq
		mov ecx, 0x64
		idiv ecx
		pop edi
		pop esi
		pop ebx
		jmp [ArtificierHealthNoHero_return]
	}
}
