#include "pch.h"
#include "types/HeroSpecs.h"

// Exhauster also reduce speed by 1

void ExhausterFork();

int Exhauster_fork = 0x008A2063;
int Exhauster_return = 0x008A2074;
int Exhauster_end = 0x008A207F;

void ExhausterTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Exhauster_fork, 13, ExhausterFork, 0, 0, 0 });
}

static bool __fastcall exhauster_slows(ICombatArmy* enemy_side) {
	ICombatHero* hero = side_hero_unit(enemy_side);
	return hero && hero->has_spec(HERO_SPEC_PRUDENT);
}

// eax = enemy side of the creature (CombatUnit vtable +0x10)
__declspec(naked) void ExhausterFork() {
	__asm
	{
		mov ecx, eax
		call exhauster_slows
		test al, al
		jz EXHAUSTER_END
		jmp [Exhauster_return]

	EXHAUSTER_END:
		jmp [Exhauster_end]
	}
}
