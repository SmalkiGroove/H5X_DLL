#include "pch.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "Artfset.h"

// ACADEMY ARTIFACT SET (15)
// "Casters" = the hero's creatures with mana (CombatUnit +0x238 > 0). "Mages" = Mage, Archmage, Battle Mage.
// 4: the hero's spellpower is increased by 2% per knowledge point (ModEffectiveSpellpower.cpp), casters' spellpower
//    by 2% per maximum mana point (ModCasterCreatures.cpp)
// 4 Academy: the hero (ModAtbReset.cpp) and the mages use 15% less ATB to cast a spell: ATB after the cast =
//    a + (1 - a) x 15%, a = vanilla ATB after the cast (0 for creatures), like the vanilla Sar-Issus set bonus
// 6: the hero and the casters start the combat with +0.5 ATB (max 1); casters' spells cost 4 less mana (min 1, like
//    the Ring of the Magister for the hero) and ignore the targets' magic resistance (like the Staff of the Magister:
//    immunities still apply)
// 6 Academy: mages and the hero's creatures adjacent to a mage take 20% less damage from attacks
//    (ModDamageOutput.cpp)

void AcademyCreatureStartAtbFork();
void AcademyHeroStartAtbFork();
void AcademyCreatureCastAtbFork();
void AcademyCasterManaFork();
void AcademyCasterResistFork();

// func_InitCombatEffects (creature combat start), owner hero present (ebp = NWorld::ICombatHero, esi = CCombatCreature),
// after the initial ATB and Agility, before the hero spec 0x19 ATB bonus
int AcademyCreatureStartAtb_fork = 0x008AB2A1;    // mov eax,[ebp] / mov ecx,ebp / call [eax] (-> IHero)
int AcademyCreatureStartAtb_return = 0x008AB2A8;
// func_InitCombat (hero combat start, esi = CCombatHero: [esi+0x44] ATB info, [esi+0xB0] IHero), after skill 0x9B
int AcademyHeroStartAtb_fork = 0x00BC22A2;        // mov ecx,[esi+0B0h]
int AcademyHeroStartAtb_return = 0x00BC22A8;
// func_CombatCreature_OnSpellCast (0x008A11C0), not a rune: ATB info of the caster set to 0 (ecx = caster CombatUnit)
int AcademyCreatureCastAtb_fork = 0x008A134B;     // push 0 / call [eax+184h]
int AcademyCreatureCastAtb_return = 0x008A1353;   // mov ecx,eax / call func_SetATBInfoValue
// func_ComputeUnitManaCost (0x009781E0), after the creature cost reductions (esi = caster, edi = [esp+0x10] = cost)
int AcademyCasterMana_fork = 0x009783CD;          // cmp dword ptr [esp+18h],2 (also the EliteCastersFix.cpp target)
int AcademyCasterMana_return = 0x009783D2;        // jne (flags of the cmp)
// func_ComputeSpellHitChance: Staff of the Magister (45) on the caster's hero skips the resistance bonuses
// (eax = hero inventory, [esp+0x30] = caster). Just after SecretsOfDestruct.cpp (0x0097CF8D).
int AcademyCasterResist_fork = 0x0097CF9A;        // push 2Dh / mov ecx,eax / call func_CountEquippedArtifact
int AcademyCasterResist_return = 0x0097CFA3;      // test eax,eax / jne 0x0097D0B0 (no resistance)

enum AcademyMage {
	CREATURE_MAGE = 63,
	CREATURE_ARCH_MAGE = 64,
	CREATURE_BATTLE_MAGE = 162,
};

static const float academy_spellpower_per_point = 0.02f;  // per knowledge (hero) / max mana (casters)
static const float academy_cast_atb_saved = 0.15f;
static const float academy_starting_atb = 0.5f;
static const int academy_mana_reduction = 4;
static const float academy_damage_taken = 0.8f;

void ArtfsetAcademy_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, AcademyCreatureStartAtb_fork, 7, AcademyCreatureStartAtbFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, AcademyHeroStartAtb_fork, 6, AcademyHeroStartAtbFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, AcademyCreatureCastAtb_fork, 8, AcademyCreatureCastAtbFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, AcademyCasterMana_fork, 5, AcademyCasterManaFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, AcademyCasterResist_fork, 9, AcademyCasterResistFork, 0, 0, 0 });
}

static int academy_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_ACADEMY) : 0;
}

static bool is_academy_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_ACADEMY_MIGHT || hero_class == HERO_CLASS_ACADEMY_MAGIC;
}

static bool is_caster(ICombatUnit* unit) {
	return unit->combat_stack() && unit->max_mana() > 0;
}

static bool is_mage(ICombatUnit* unit) {
	ICreature* creature = ((ICombatCreature*)unit)->creature();
	if (!creature) return false;
	switch (creature->type_id) {
	case CREATURE_MAGE:
	case CREATURE_ARCH_MAGE:
	case CREATURE_BATTLE_MAGE:
		return true;
	}
	return false;
}

// Owner hero with at least `pieces` pieces (and the Academy class when `academy`), else nullptr.
static IHero* academy_hero(ICombatUnit* unit, int pieces, bool academy) {
	IHero* hero = owner_hero(unit);
	if (academy_pieces(hero) < pieces || (academy && !is_academy_class(hero))) return nullptr;
	return hero;
}

// ATB info +0x1C raised by the starting bonus, up to 1 (never lowered).
static void add_starting_atb(int* atb_info) {
	float atb = *(float*)(atb_info + 0x1C / 4);
	if (atb >= 1.0f) return;
	atb += academy_starting_atb;
	set_atb_info_value(atb_info, atb > 1.0f ? 1.0f : atb);
}

static float cast_atb(float atb) {
	return atb < 1.0f ? atb + (1.0f - atb) * academy_cast_atb_saved : atb;
}

int artfset_academy_hero_spellpower(IHero* hero, int spellpower) {
	if (academy_pieces(hero) < 4) return spellpower;
	return (int)(spellpower * (1.0f + academy_spellpower_per_point * hero->knowledge()));
}

int artfset_academy_caster_spellpower(ICombatUnit* unit, int spellpower) {
	if (!academy_hero(unit, 4, false)) return spellpower;
	int mana = unit->max_mana();
	if (mana <= 0) return spellpower;
	return (int)(spellpower * (1.0f + academy_spellpower_per_point * mana));
}

float artfset_academy_hero_cast_atb(IHero* hero, float atb) {
	if (academy_pieces(hero) < 4 || !is_academy_class(hero)) return atb;
	return cast_atb(atb);
}

// Adjacent: the size x size squares of the two units touch, diagonals included.
static bool adjacent(ICombatUnit* a, ICombatUnit* b) {
	int pa[4] = { 0 }, pb[4] = { 0 };
	a->position(pa);
	b->position(pb);
	int sa = a->size(), sb = b->size();
	// gap between the squares on each axis: 1 = touching
	return pb[0] - sb + 1 - pa[0] <= 1 && pa[0] - sa + 1 - pb[0] <= 1
		&& pb[1] - sb + 1 - pa[1] <= 1 && pa[1] - sa + 1 - pb[1] <= 1;
}

static bool next_to_mage(ICombatUnit* defender) {
	int* combat = defender->combat();
	if (!combat) return false;
	int* side = defender->unit_ref();
	int units[3] = { 0, 0, 0 };
	collect_all_combat_units(combat, units);
	bool found = false;
	for (char** entry = (char**)units[0]; entry != (char**)units[1] && !found; ++entry) {
		char* object = *entry;
		if (!object) continue;
		ICombatUnit* unit = (ICombatUnit*)(object + 4 + *(int*)(*(int*)(object + 4) + 8));
		if (unit == defender || !unit->combat_stack() || unit->is_dead() || unit->unit_ref() != side) continue;
		found = is_mage(unit) && adjacent(unit, defender);
	}
	if (units[0]) engine_free((void*)units[0]);
	return found;
}

float artfset_academy_damage_taken(ICombatUnit* defender) {
	if (!defender->combat_stack() || !academy_hero(defender, 6, true)) return 1.0f;
	return is_mage(defender) || next_to_mage(defender) ? academy_damage_taken : 1.0f;
}

// creature = CCombatCreature (CombatUnit at +4 + [[+4]+8]), hero = its owner
static void __fastcall academy_creature_start_atb(char* creature, IHero* hero) {
	if (academy_pieces(hero) < 6) return;
	ICombatUnit* unit = (ICombatUnit*)(creature + 4 + *(int*)(*(int*)(creature + 4) + 8));
	if (is_caster(unit)) add_starting_atb(unit->atb_info());
}

static void __fastcall academy_hero_start_atb(IHero* hero, int* atb_info) {
	if (academy_pieces(hero) >= 6) add_starting_atb(atb_info);
}

// ATB after a creature's cast (vanilla 0)
static float __fastcall academy_creature_cast_atb(ICombatUnit* caster) {
	if (!academy_hero(caster, 4, true) || !is_mage(caster)) return 0.0f;
	return cast_atb(0.0f);
}

static int __fastcall academy_caster_mana(ICombatUnit* caster, int cost) {
	if (cost <= 1 || !caster->combat_stack() || !academy_hero(caster, 6, false)) return cost;
	cost -= academy_mana_reduction;
	return cost < 1 ? 1 : cost;
}

// Non-zero: the caster's spells skip the resistance bonuses
static int __fastcall academy_caster_unresisted(ICombatUnit* caster) {
	return caster->combat_stack() && academy_hero(caster, 6, false);
}

// ebp = NWorld::ICombatHero (slot 0 -> IHero, kept in eax), esi = CCombatCreature
__declspec(naked) void AcademyCreatureStartAtbFork() {
	__asm
	{
		mov eax, dword ptr [ebp]
		mov ecx, ebp
		call dword ptr [eax]
		push eax
		mov edx, eax
		mov ecx, esi
		call academy_creature_start_atb
		pop eax
		jmp [AcademyCreatureStartAtb_return]
	}
}

__declspec(naked) void AcademyHeroStartAtbFork() {
	__asm
	{
		mov ecx, dword ptr [esi + 0xB0]
		mov edx, dword ptr [esi + 0x44]
		call academy_hero_start_atb
		mov ecx, dword ptr [esi + 0xB0]
		jmp [AcademyHeroStartAtb_return]
	}
}

// ecx = caster CombatUnit: pushes the ATB value, then the vanilla call for the ATB info
__declspec(naked) void AcademyCreatureCastAtbFork() {
	__asm
	{
		push ecx
		call academy_creature_cast_atb
		pop ecx
		sub esp, 0x4
		fstp dword ptr [esp]
		mov eax, dword ptr [ecx]
		call dword ptr [eax + 0x184]
		jmp [AcademyCreatureCastAtb_return]
	}
}

__declspec(naked) void AcademyCasterManaFork() {
	__asm
	{
		mov ecx, esi
		mov edx, edi
		call academy_caster_mana
		mov edi, eax
		mov dword ptr [esp + 0x10], edi
		cmp dword ptr [esp + 0x18], 0x2
		jmp [AcademyCasterMana_return]
	}
}

__declspec(naked) void AcademyCasterResistFork() {
	__asm
	{
		push ARTIFACT_STAFF_OF_THE_MAGISTER
		mov ecx, eax
		call [count_equipped_artifact]
		test eax, eax
		jnz ACADEMY_RESIST_END
		mov ecx, dword ptr [esp + 0x30]
		call academy_caster_unresisted

	ACADEMY_RESIST_END:
		jmp [AcademyCasterResist_return]
	}
}
