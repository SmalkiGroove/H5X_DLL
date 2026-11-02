#include "pch.h"
#include "types/HeroSpecs.h"

// Infantry commander attacks decrease target's morale

void InfantryCommanderFork();

int InfantryCommander_fork = 0x008CF7F3;
int InfantryCommander_return = 0x008CF81F;
int InfantryCommander_end = 0x008CF875;

void InfantryCommanderTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, InfantryCommander_fork, 44, InfantryCommanderFork, 0, 0, 0 });
}

// hero = attacking hero (ICombatHeroInterface, vbptr at +4)
static bool __fastcall infantry_commander_attack(int* hero) {
	return combat_creature_detail::combat_hero_from_outer(hero)->has_spec(HERO_SPEC_FOOTMEN);
}

// esi = attacking hero (ICombatHeroInterface)
__declspec(naked) void InfantryCommanderFork() {
	__asm
	{
		mov ecx, esi
		call infantry_commander_attack
		test al, al
		je INFANTRY_COMANDER_END
		jmp [InfantryCommander_return]

	INFANTRY_COMANDER_END:
		jmp [InfantryCommander_end]
	}
}
