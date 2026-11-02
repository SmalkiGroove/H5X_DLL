#include "pch.h"

// Perception of Weakness increases spell damage by 15%

void PerceptionOfWeaknessFork();

int PerceptionOfWeakness_fork = 0x004B0E43;
int PerceptionOfWeakness_return = 0x004B0E49;

void PerceptionOfWeakness_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PerceptionOfWeakness_fork, 6, PerceptionOfWeaknessFork, 0, 0, 0 });
}

// object = the spell damage function's this (esi, vbptr at +4); the caster is the hero of its unit's enemy side
static bool __fastcall perception_of_weakness_active(void* object) {
	ICombatUnit* unit = combat_creature_detail::combat_unit_from_outer(object);
	ICombatHero* caster = side_hero_unit(unit->enemy_side());
	return caster && caster->has_perk(PERK_PERCEPTION_OF_WEAKNESS);
}

// [esp] = damage factor (float)
__declspec(naked) void PerceptionOfWeaknessFork() {
	__asm
	{
		mov ecx, esi
		call perception_of_weakness_active
		test al, al
		je PERCEPTION_OF_WEAKNESS_RETURN
		fld dword ptr [esp]
		fadd dword ptr [constf_0_15]
		fstp dword ptr [esp]

	PERCEPTION_OF_WEAKNESS_RETURN:
		mov ecx, dword ptr [esi + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		jmp [PerceptionOfWeakness_return]
	}
}
