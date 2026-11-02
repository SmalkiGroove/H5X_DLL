#include "pch.h"

// Elementalist doubles elemental chains damages

void ElementalistFork();

int Elementalist_fork = 0x004B0E49;
int Elementalist_return = 0x004B0E51;

void ElementalistChain_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Elementalist_fork, 8, ElementalistFork, 0, 0, 0 });
}

// object = the chain damage function's this (esi, vbptr at +4); the caster is the hero of its unit's enemy side
static bool __fastcall elementalist_doubles_chain(void* object) {
	ICombatUnit* unit = combat_creature_detail::combat_unit_from_outer(object);
	ICombatHero* caster = side_hero_unit(unit->enemy_side());
	return caster && caster->has_perk(SPEC_FORBIDDEN_POWER);
}

// [esp] = damage (float); runs after PerceptionOfWeaknessFork, which leaves edx = [[esi+4]+8]
__declspec(naked) void ElementalistFork() {
	__asm
	{
		mov ecx, esi
		call elementalist_doubles_chain
		test al, al
		jz ELEMENTALIST_RETURN
		fld dword ptr [esp]
		fadd st(0), st(0)
		fstp dword ptr [esp]

	ELEMENTALIST_RETURN:
		mov ecx, dword ptr [esi + 0x4]
		mov edx, dword ptr [ecx + 0x8]
		mov eax, dword ptr [edx + esi + 0x4]
		lea ecx, dword ptr [edx + esi + 0x4]
		jmp [Elementalist_return]
	}
}
