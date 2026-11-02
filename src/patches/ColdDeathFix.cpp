#include "pch.h"
#include "types/Abilities.h"

// COLD DEATH SHOULD NOT APPLY ON MECHANICAL AND CRYSTAL SCALES UNITS
// (nor on units whose own hero wears Dragon Flame Tongue)

void ColdDeathFork();

int ColdDeath_fork = 0x00977AE2;
int ColdDeath_return = 0x00977AEC;
int ColdDeath_end = 0x00977AF0;

void ColdDeathFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ColdDeath_fork, 10, ColdDeathFork, 0, 0, 0 });
}

static bool __fastcall cold_death_immune(ICombatUnit* target) {
	if (target->has_ability(ABILITY_CRYSTAL_SCALES) || target->has_ability(ABILITY_MECHANICAL)) return true;
	IHero* hero = owner_hero(target);
	return hero && hero->has_artifact(ARTIFACT_DRAGON_FLAME_TONGUE);
}

// esi = target unit
__declspec(naked) void ColdDeathFork() {
	__asm
	{
		mov ecx, esi
		call cold_death_immune
		test al, al
		jne FUNCTION_END

		mov edx, dword ptr [esi]
		mov ecx, esi
		call dword ptr [edx]CombatUnit_vtable.get_creature_max_hp
		jmp [ColdDeath_return]

	FUNCTION_END:
		jmp [ColdDeath_end]
	}
}
