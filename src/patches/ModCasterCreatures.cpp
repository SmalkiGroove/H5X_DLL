#include "pch.h"
#include "types/HeroSpecs.h"
#include "Artfset.h"

// CASTER CREATURES: mana and spellpower of the creatures.
//
// Base spellpower (func_CreatureSpellpowerFormula, 0x004BAD20, replaced): scales linearly with the stack size,
// 1 + 2.5 per weekly growth of creatures: 1 + (10 * count / WeeklyGrowth) / 4 (integer divisions).
// Vanilla: round(log10(10 * count / WeeklyGrowth + 10) * a - b + c), min 1.
//
// Caster creatures have twice their mana and spellpower (DefaultStats +0x117C multiplier) with:
// - the Researcher spec (Shaman Matriarch),
// - 4 pieces of the Academy set (ArtfsetAcademy.cpp).
// Both sites are the vanilla Sar-Issus set (4) bonus with 2 pieces.

int __fastcall creature_spellpower_formula(int creature_id, int count);
void CasterManaFork();
void CasterSpellpowerFork();

int CasterSpellpowerFormula_fork = 0x004BAD20;  // func_CreatureSpellpowerFormula entry (only caller 0x008A3AE5): sub esp,8 / push esi / mov esi,edx

int CasterMana_fork = 0x004BAB46;               // func_GetCreatureManaPoints, ecx = IHero: push 4 / call [eax+328h] / cmp eax,2 / jl
int CasterMana_return = 0x004BAB53;             // mana * DefaultStats +0x117C
int CasterMana_end = 0x004BAB65;                // base mana

int CasterSpellpower_fork = 0x008A3B01;         // creature spellpower, eax = side: mov edx,4 / mov ecx,eax / call func_CountArtifactSetPieces / cmp eax,2 / jl
int CasterSpellpower_return = 0x008A3B12;       // spellpower * DefaultStats +0x117C, tooltip notification of set 4 (0x009BBA20)
int CasterSpellpower_end = 0x008A3B35;          // edi = base spellpower, [esp+0xC] = spellpower

void CasterCreatures_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, CasterSpellpowerFormula_fork, 6, (void*)creature_spellpower_formula, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, CasterMana_fork, 13, CasterManaFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, CasterSpellpower_fork, 17, CasterSpellpowerFork, 0, 0, 0 });
}

// Same calling convention as the replaced function: the hook jumps here with the caller's return address on the stack.
int __fastcall creature_spellpower_formula(int creature_id, int count) {
	int* data = get_creature_data(creature_id);
	if (!data) return 0;
	int growth = data[0xA8 / 4];  // WeeklyGrowth
	if (growth < 1) growth = 1;
	return count * 10 / growth / 4 + 1;
}

static int __fastcall caster_mana_doubled(IHero* hero) {
	return hero->has_spec(HERO_SPEC_RESEARCHER) || artfset_academy_casters(hero);
}

enum CasterSpellpowerBonus {
	CASTER_SPELLPOWER_NONE = 0,
	CASTER_SPELLPOWER_SET = 1,     // Academy set: no tooltip notification, like the other H5X sets
	CASTER_SPELLPOWER_SPEC = 2,    // Researcher: vanilla path and notification
};

// side = side of the creature (CombatUnit +0x0C)
static int __fastcall caster_spellpower_doubled(int* side) {
	using namespace combat_creature_detail;
	ICombatHero* hero = combat_hero_from_outer(call_unit_ref(side));
	if (!hero) return CASTER_SPELLPOWER_NONE;
	if (hero->has_spec(HERO_SPEC_RESEARCHER)) return CASTER_SPELLPOWER_SPEC;
	if (artfset_academy_casters(hero->hero())) return CASTER_SPELLPOWER_SET;
	return CASTER_SPELLPOWER_NONE;
}

// ecx = IHero (not null), esi = base mana
__declspec(naked) void CasterManaFork() {
	__asm
	{
		call caster_mana_doubled
		test eax, eax
		jz CASTER_MANA_END
		jmp [CasterMana_return]

	CASTER_MANA_END:
		jmp [CasterMana_end]
	}
}

// eax = side, edi = base spellpower
__declspec(naked) void CasterSpellpowerFork() {
	__asm
	{
		mov ecx, eax
		call caster_spellpower_doubled
		cmp eax, CASTER_SPELLPOWER_SPEC
		je CASTER_SPELLPOWER_SPEC_PATH
		test eax, eax
		jz CASTER_SPELLPOWER_END

		call [get_defaultstats]
		mov eax, dword ptr [eax + 0x117C]
		imul eax, edi
		mov dword ptr [esp + 0xC], eax

	CASTER_SPELLPOWER_END:
		jmp [CasterSpellpower_end]

	CASTER_SPELLPOWER_SPEC_PATH:
		jmp [CasterSpellpower_return]
	}
}
