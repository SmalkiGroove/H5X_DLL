#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"
#include "structs/Spell.h"
#include "types/Abilities.h"

// PERK_43 (Shocking Bolts): lightning damage spells consume the target's retaliation (+0xC8).

void ShockingBoltsTargetedFork();

int ShockingBoltsTargeted_fork = 0x0097DF6C;
int ShockingBoltsTargeted_return = 0x0097E860;

static bool __cdecl ShouldApplyShockingBolts(int spell_id, int caster_outer);
static void __cdecl ConsumeTargetRetaliation(int target_outer);
static void __cdecl TryShockingBoltsRetaliation(int target_outer, int caster_outer, int spell_id);

void ShockingBolts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShockingBoltsTargeted_fork, 7, ShockingBoltsTargetedFork, 0, 0, 0 });
}

static bool __cdecl ShouldApplyShockingBolts(int spell_id, int caster_outer) {
	if (!spell_id || !caster_outer)
		return false;

	ICombatHero* combatHero = combat_creature_detail::combat_hero_from_outer((int*)caster_outer);
	if (!combatHero || !combatHero->has_skill(PERK_43))
		return false;

	int element = 0;
	__asm {
		mov ecx, spell_id
		call get_spell_element
		mov element, eax
	}
	return element == SPELL_ELEMENT_LIGHTNING;
}

static void __cdecl ConsumeTargetRetaliation(int target_outer) {
	if (!target_outer)
		return;

	CombatUnit_vtable* unit_vt = *(CombatUnit_vtable**)target_outer;
	if (((ThiscallInt_IntPtr)unit_vt->get_state_flag_7c)((int*)target_outer))
		return;

	ICombatCreature* unit = (ICombatCreature*)target_outer;
	if (unit->has_ability(ABILITY_UNLIMITED_RETALIATION))
		return;

	*(int*)(target_outer + 0xC8) = 1;
}

static void __cdecl TryShockingBoltsRetaliation(int target_outer, int caster_outer, int spell_id) {
	if (!ShouldApplyShockingBolts(spell_id, caster_outer))
		return;
	ConsumeTargetRetaliation(target_outer);
}

__declspec(naked) void ShockingBoltsTargetedFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		mov esi, dword ptr [esp + 0x1C]
		mov ebx, dword ptr [esp + 0x98]
		push dword ptr [esp + 0x4]
		push ebx
		push esi
		call TryShockingBoltsRetaliation
		add esp, 12

		pop eax
		pop edi
		pop esi
		pop ebx
		mov ebp, eax
		jmp[ShockingBoltsTargeted_return]
	}
}
