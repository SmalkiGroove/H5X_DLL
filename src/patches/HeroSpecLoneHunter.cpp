#include "pch.h"

// Lone Hunter restores 30% ATB after attacks

void LoneHunterFork();

int LoneHunter_fork = 0x008CE8FB;
int LoneHunter_return = 0x008CE917;
int LoneHunter_end = 0x008CE96A;

void LoneHunterTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, LoneHunter_fork, 28, LoneHunterFork, 0, 0, 0 });
}

static bool __fastcall lone_hunter_restores_atb(IHero* hero) {
	return (char)hero->skill_mastery(SPEC_RAIN_OF_ARROWS) != 0;
}

// ecx = attacking IHero
__declspec(naked) void LoneHunterFork() {
	__asm
	{
		call lone_hunter_restores_atb
		test al, al
		jz LONEHUNTER_END
		jmp [LoneHunter_return]

	LONEHUNTER_END:
		jmp [LoneHunter_end]
	}
}
