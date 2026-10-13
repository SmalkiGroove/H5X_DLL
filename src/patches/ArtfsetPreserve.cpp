#include "pch.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "types/Spells.h"
#include "Artfset.h"

// SYLVAN ARTIFACT SET (12)
// 4: hero's effective spellpower is doubled for nature spells (applied by ModEffectiveSpellpower.cpp)
// 4 Sylvan: Sylvan creatures deal +20% damage to the hero's favored enemies (added to Forest Guardian's bonus)
// 6: creatures always roll good luck (replaces the vanilla Supernatural Luck skill check, the enemy's skill 0xA7
//    still cancels it)
// 6 Sylvan: creatures take 5% less spell damage per positive luck point (DefaultStats +0x704 per point),
//           re-enables the vanilla Luck of the Barbarian block (skill 0xB3) with the set check instead

void ArtfsetSylvanFavoredFork();
void ArtfsetSylvanLuckFork();
void ArtfsetSylvanSpellproofFork();

static const float sylvan4_favored_bonus = 0.2f;

int ArtfsetSylvanFavored_fork = 0x00A5AE0C;       // func_ComputeDamageMitigation: call FUN_00A59340
int ArtfsetSylvanFavored_func = 0x00A59340;       // favored enemy damage multiplier (Forest Guardian), ecx = attacker, eax = target
int ArtfsetSylvanLuck_fork = 0x008A3F98;          // creature luck roll: own hero has skill 0x7C (Supernatural Luck)
int ArtfsetSylvanLuck_return = 0x008A3FB0;        // je (no forced luck) / mov edi,1 (forced good luck)
int ArtfsetSylvanSpellproof_fork = 0x0097B055;    // func_ComputeSpellDamageMitigation: skill 0xB3 check
int ArtfsetSylvanSpellproof_return = 0x0097B068;
int ArtfsetSylvanSpellproof_skip = 0x0097B0B8;

void ArtfsetPreserve_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_CALL, ArtfsetSylvanFavored_fork, 5, ArtfsetSylvanFavoredFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetSylvanLuck_fork, 24, ArtfsetSylvanLuckFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetSylvanSpellproof_fork, 17, ArtfsetSylvanSpellproofFork, 0, 0, 0 });
	// luck > 0 ? 1 : 0 -> max(luck, 0): test eax,eax / jg +2 / xor eax,eax / nop x3
	assembly_patches.push_back({ PATCH_WRTE, 0x0097B072, 9, nullptr, 0, 0, 0, "85C07F0233C0909090" });
}

static int sylvan_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_SYLVAN) : 0;
}

static bool is_sylvan_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_PRESERVE_MIGHT || hero_class == HERO_CLASS_PRESERVE_MAGIC;
}

static bool is_favored_enemy(IHero* hero, ICreature* creature) {
	typedef char(__thiscall* ContainsFn)(int*, int);
	int* favored = hero->favored_enemies();
	if (!favored) return false;
	return ((ContainsFn)(*(int***)favored)[0x0C / 4])(favored, creature->type_id) != 0;
}

bool artfset_sylvan_spellpower_doubled(IHero* hero, int* spell) {
	return get_spell_school(spell) == SPELL_SCHOOL_NATURAL && sylvan_pieces(hero) >= 4;
}

static float __fastcall sylvan4_favored_multiplier(ICombatUnit* attacker, ICombatUnit* target, float multiplier) {
	if (!attacker || !target) return multiplier;
	IHero* hero = owner_hero(attacker);
	if (sylvan_pieces(hero) < 4 || !is_sylvan_class(hero)) return multiplier;
	ICreature* attacker_creature = ((ICombatCreature*)attacker)->creature();  // nullptr for heroes
	ICreature* target_creature = ((ICombatCreature*)target)->creature();
	if (!attacker_creature || !target_creature) return multiplier;
	if (get_creature_town((int*)attacker_creature) != 4) return multiplier;
	if (!is_favored_enemy(hero, target_creature)) return multiplier;
	return multiplier + sylvan4_favored_bonus;
}

static int __fastcall sylvan6_always_lucky(ICombatUnit* unit) {
	return sylvan_pieces(owner_hero(unit)) >= 6;
}

static int __fastcall sylvan6_luck_spellproof(IHero* hero) {
	return sylvan_pieces(hero) >= 6 && is_sylvan_class(hero);
}

// Replaces the call: ecx = attacker, eax = target, returns the multiplier in st0
__declspec(naked) void ArtfsetSylvanFavoredFork() {
	__asm
	{
		push ecx
		push eax
		call [ArtfsetSylvanFavored_func]
		pop edx
		pop ecx
		sub esp, 0x4
		fstp dword ptr [esp]
		call sylvan4_favored_multiplier
		ret
	}
}

// esi = creature CombatUnit subobject, edi = own combat hero (not null); ZF clear forces good luck
__declspec(naked) void ArtfsetSylvanLuckFork() {
	__asm
	{
		mov ecx, esi
		call sylvan6_always_lucky
		test eax, eax
		jmp [ArtfsetSylvanLuck_return]
	}
}

// ebx = IHero of the defending side, edi = defending unit (not null)
__declspec(naked) void ArtfsetSylvanSpellproofFork() {
	__asm
	{
		mov ecx, ebx
		call sylvan6_luck_spellproof
		test eax, eax
		je SYLVAN_SPELLPROOF_SKIP
		jmp [ArtfsetSylvanSpellproof_return]

	SYLVAN_SPELLPROOF_SKIP:
		jmp [ArtfsetSylvanSpellproof_skip]
	}
}
