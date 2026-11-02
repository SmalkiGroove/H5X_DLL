#include "pch.h"
#include "types/HeroSpecs.h"

// Temptress gives shooters atb

void TemptressFork();

int Temptress_fork = 0x00A65C35;
int Temptress_return = 0x00A65C4D;
int Temptress_end = 0x00A65CE1;

void TemptressTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Temptress_fork, 24, TemptressFork, 0, 0, 0 });
}

// unit = the shooter; its own hero must be a Temptress
static bool __fastcall temptress_gives_atb(ICombatUnit* unit) {
	ICombatHero* hero = side_hero_unit(unit->side());
	return hero && hero->has_spec(HERO_SPEC_SUCCUBUSES);
}

// ecx = shooter unit
__declspec(naked) void TemptressFork() {
	__asm
	{
		call temptress_gives_atb
		test al, al
		jz TEMPTRESS_END
		jmp [Temptress_return]

	TEMPTRESS_END:
		jmp [Temptress_end]
	}
}
