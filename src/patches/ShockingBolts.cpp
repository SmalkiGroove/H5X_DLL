#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"

// PERK_SHOCKING_BOLTS (Shocking Bolts): lightning damage spells consume the target's retaliation (+0xC8).

void ShockingBoltsFork();

int ShockingBolts_fork = 0x004E6B19;
int ShockingBolts_return = 0x004E6B1E;

static void __cdecl TryShockingBolts(int caster_outer, int target_outer);

void ShockingBolts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShockingBolts_fork, 5, ShockingBoltsFork, 0, 0, 0 });
}

static void __cdecl TryShockingBolts(int caster_outer, int target_outer) {
	if (!caster_outer || !target_outer)
		return;

	ICombatHero* combatHero = combat_creature_detail::combat_hero_from_outer((int*)caster_outer);
	if (!combatHero || !combatHero->has_skill(PERK_SHOCKING_BOLTS))
		return;

	CombatUnit_vtable* unit_vt = *(CombatUnit_vtable**)target_outer;
	if (unit_vt->get_state_flag_7c((int*)target_outer))
		return;

	*(int*)(target_outer + 0xC8) = 1;
}

__declspec(naked) void ShockingBoltsFork() {
	__asm {
		push ebx
		push ebp
		push edi
		push esi

		push ebp
		push edi
		call TryShockingBolts
		add esp, 8

		pop esi
		pop edi
		pop ebp
		pop ebx

		push esi
		mov edx, ebp
		mov ecx, edi
		jmp[ShockingBolts_return]
	}
}
