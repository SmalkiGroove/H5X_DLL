#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"
#include "structs/Spell.h"

// PERK_UNSETTLE: earth damage spells apply a 10% max-HP penalty per damaged stack

void UnsettleTargetedFork();
void UnsettleAreaFork();

int UnsettleTargeted_fork = 0x0097DF67;
int UnsettleTargeted_return = 0x0097DF6C;
int UnsettleArea_fork = 0x0085ECE2;
int UnsettleArea_return = 0x0085ECE7;

static bool __cdecl ShouldApplyUnsettle(int spell_id, int caster_outer);
static void __cdecl DoApplyUnsettle(int target, int timeline);

void Unsettle_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, UnsettleTargeted_fork, 5, UnsettleTargetedFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, UnsettleArea_fork, 5, UnsettleAreaFork, 0, 0, 0 });
}

static bool __cdecl ShouldApplyUnsettle(int spell_id, int caster_outer) {
	if (!spell_id || !caster_outer)
		return false;

	ICombatHero* combatHero = combat_creature_detail::combat_hero_from_outer((int*)caster_outer);
	if (!combatHero || !combatHero->has_skill(PERK_UNSETTLE))
		return false;

	int element = 0;
	__asm {
		mov ecx, spell_id
		call get_spell_element
		mov element, eax
	}
	return element == SPELL_ELEMENT_EARTH;
}

static void __cdecl DoApplyUnsettle(int target_outer, int timeline) {
	CombatUnit_vtable* unit_vt = *(CombatUnit_vtable**)target_outer;
	int* side_ptr = ((GetUnitRef)unit_vt->get_side_ptr_a)((int*)target_outer);
	if (!side_ptr)
		return;

	void** side_vt = *(void***)side_ptr; // should be 0x00E4FE9C
	((ThiscallVoid_IntPtr_Int_Float)side_vt[0x4C / sizeof(void*)])(side_ptr, timeline, constf_0_1);
}

__declspec(naked) void UnsettleTargetedFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		push esi
		push dword ptr [esp + 0x1C]
		call ShouldApplyUnsettle
		add esp, 8
		test al, al
		je UNSETTLE_TARGET_SKIP

		push ebp
		push edi
		call DoApplyUnsettle
		add esp, 8

		UNSETTLE_TARGET_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, edi
		mov ecx, esi
		call apply_spell_damage
		jmp[UnsettleTargeted_return]
	}
}

__declspec(naked) void UnsettleAreaFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		push esi
		push dword ptr [esp + 0x1C]
		call ShouldApplyUnsettle
		add esp, 8
		test al, al
		je UNSETTLE_AREA_SKIP

		push ebx
		push ebp
		call DoApplyUnsettle
		add esp, 8

		UNSETTLE_AREA_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, ebp
		mov ecx, esi
		call apply_spell_damage
		jmp[UnsettleArea_return]
	}
}
