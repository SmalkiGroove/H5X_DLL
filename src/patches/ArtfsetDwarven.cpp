#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "types/Spells.h"
#include "Artfset.h"

// DWARVEN ARTIFACT SET (17)
// 4: hero's creatures get the Offensive and Defensive Formation bonuses; doubled when the hero has the perk
//   (OffensiveDefensiveFormation.cpp)
// 4 Fortress: hero's creatures of base 92, 94, 96, 98, 100, 102 (and their upgrades) have Presence of Commander and
//   Aura of Bravery (shared has_ability hook, ModAbilities.cpp)
// 6: hero's creatures take 50% less spell damage (ModSpellProof.cpp) and get +5 maximum HP (ModHealth.cpp)
// 6 Fortress: runes are not consumed when activated (hooks below); their resource cost is still paid

void RuneChargeCastFork();
void RuneChargeActivateFork();

int RuneChargeCast_fork = 0x00A6A6D0;        // func_ConsumeSpellCharge (thiscall(spell ref), ret 4): consumes a charge after a cast (creature / hero on_spell_cast)
int RuneChargeActivate_fork = 0x00A6A9B2;    // FUN_00a6a970: consumes a charge (unit +0x2B8 list, func_SpellCharges_Consume)
int RuneChargeActivate_return = 0x00A6A9D5;  // mov al,1

static const int dwarven_commanders[] = { 92, 94, 96, 98, 100, 102 };  // base creatures
static const int dwarven6_health = 5;

typedef void(__thiscall* ConsumeSpellCharge)(int*, int);
static ConsumeSpellCharge consume_spell_charge = (ConsumeSpellCharge)0x00989270;  // thiscall(charge list, spell ID)

void ArtfsetDwarven_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, RuneChargeCast_fork, 6, RuneChargeCastFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, RuneChargeActivate_fork, 35, RuneChargeActivateFork, 0, 0, 0 });
}

static int dwarven_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_DWARVEN) : 0;
}

static bool is_fortress_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_FORTRESS_MIGHT || hero_class == HERO_CLASS_FORTRESS_MAGIC;
}

int artfset_dwarven_formation(IHero* hero) {
	return dwarven_pieces(hero) >= 4 ? 1 : 0;
}

bool artfset_dwarven_spell_proof(IHero* hero) {
	return dwarven_pieces(hero) >= 6;
}

int artfset_dwarven_health(IHero* hero) {
	return dwarven_pieces(hero) >= 6 ? dwarven6_health : 0;
}

static bool is_dwarven_commander(ICombatUnit* unit) {
	ICreature* creature = ((ICombatCreature*)unit)->creature();
	if (!creature) return false;
	int base = creature->base_type_id();
	for (int id : dwarven_commanders)
		if (base == id) return true;
	return false;
}

// Abilities 116 / 155, through the shared has_ability hook (ModAbilities.cpp)
int artfset_dwarven_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_PRESENCE_OF_COMMANDER && ability != ABILITY_AURA_OF_BRAVERY) return 0;
	IHero* hero = owner_hero(unit);
	if (dwarven_pieces(hero) < 4 || !is_fortress_class(hero)) return 0;
	return is_dwarven_commander(unit);
}

// self = `this` of func_ConsumeSpellCharge / FUN_00a6a970 (vbptr at self+4, virtual base = the CombatUnit subobject).
// Replaces: unit +0x2B8 (charge list) -> func_SpellCharges_Consume(spell).
static void __fastcall consume_charge(char* self, int spell) {
	int** vbtable = *(int***)(self + 4);
	ICombatUnit* unit = (ICombatUnit*)(self + 4 + (int)vbtable[2]);
	if (get_spell_school((int*)spell) == SPELL_SCHOOL_RUNIC) {
		IHero* hero = owner_hero(unit);
		if (dwarven_pieces(hero) >= 6 && is_fortress_class(hero)) return;
	}
	consume_spell_charge(unit->instance->get_spell_charges((int*)unit), spell);
}

// Replaces the whole function: ecx = self, [esp+4] = spell ref (+4 = spell ID)
__declspec(naked) void RuneChargeCastFork() {
	__asm
	{
		mov eax, dword ptr [esp + 0x4]
		mov edx, dword ptr [eax + 0x4]
		call consume_charge
		ret 4
	}
}

// esi = self, [esp+0xC] = spell ref
__declspec(naked) void RuneChargeActivateFork() {
	__asm
	{
		mov eax, dword ptr [esp + 0xC]
		mov edx, dword ptr [eax + 0x4]
		mov ecx, esi
		call consume_charge
		jmp [RuneChargeActivate_return]
	}
}
