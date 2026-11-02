#include "pch.h"

// ADD SKILLS TO SUMMON SPECIFIC ELEM TYPES
// Summon Elementals: Warriors of the Magma (Pyromancy ID patched in) keeps the fire elemental (0x55);
// Warriors of the Mountain / Sea / Sky summon earth / water / air elementals; otherwise the vanilla random pick.

void ElementalSummonTypeFork();

int ElemWarriors_fork = 0x00861684;
int ElemWarriors_return = 0x00861689;
int ElemWarriors_end = 0x00861696;

static const int WaterCreatureID = 0x56;
static const int EarthCreatureID = 0x57;
static const int AirCreatureID = 0x58;

void ElementalWarriors_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ElemWarriors_fork, 5, ElementalSummonTypeFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00861667, 1, nullptr, 241, 0, 0, 0 }); // Pyromancy skill ID
	assembly_patches.push_back({ PATCH_WRTE, 0x0086286F, 1, nullptr, 0, 0, 0, "EB" }); // remove 40% bonus to Pyromancy
}

// Elemental of the caster's perk, 0 without one
static int __fastcall elemental_warriors_creature(ICombatHero* caster) {
	if (caster->has_perk(PERK_WARRIORS_OF_THE_MOUNTAIN)) return EarthCreatureID;
	if (caster->has_perk(PERK_WARRIORS_OF_THE_SEA)) return WaterCreatureID;
	if (caster->has_perk(PERK_WARRIORS_OF_THE_SKY)) return AirCreatureID;
	return 0;
}

// ebp = caster hero unit; ebx = creature summoned at the end
__declspec(naked) void ElementalSummonTypeFork() {
	__asm
	{
		mov ecx, ebp
		call elemental_warriors_creature
		test eax, eax
		jne FUNCTION_RETURN

		push edi
		lea edx, dword ptr [esp + 0x1C]
		jmp [ElemWarriors_return]

	FUNCTION_RETURN:
		mov ebx, eax
		jmp [ElemWarriors_end]
	}
}
