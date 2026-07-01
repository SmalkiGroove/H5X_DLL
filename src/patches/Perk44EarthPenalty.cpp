#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"
#include "structs/Spell.h"

// PERK_UNSETTLE: earth damage spells apply a 10% max-HP penalty per damaged stack
// via side-stack vtable +0x38 (func_CombatSideStack_ApplyPercentMaxHpDelta @ 0x00887B10).
// Single-target path: func_CastSpell direct damage @ 0x0097DF67.
// Tile-area earth spells (0x8, 0xED, 0x11D) use func_CastAreaSpell @ 0x0085EAA0;
// hook per-target ApplySpellDamage @ 0x0085ECE2.

void Perk44EarthPenaltyFork();
void Perk44EarthPenaltyAreaFork();

int Perk44EarthPenalty_fork = 0x0097DF67;
int Perk44EarthPenalty_return = 0x0097DF6C;
int Perk44EarthPenalty_area_fork = 0x0085ECE2;
int Perk44EarthPenalty_area_return = 0x0085ECE7;

static bool __cdecl ShouldApplyUnsettle(int spell_id, int caster_outer);
static void __cdecl DoApplyUnsettle(int target, int timeline);

void Perk44EarthPenalty_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Perk44EarthPenalty_fork, 5, Perk44EarthPenaltyFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, Perk44EarthPenalty_area_fork, 5, Perk44EarthPenaltyAreaFork, 0, 0, 0 });
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

__declspec(naked) void Perk44EarthPenaltyFork() {
	__asm {
		push ebx
		push esi
		push edi
		push eax

		push esi
		push dword ptr [ebx + 0x4]
		call ShouldApplyUnsettle
		add esp, 8
		test al, al
		je PERK44_SKIP

		push ebp
		push edi
		call DoApplyUnsettle
		add esp, 8

		PERK44_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, edi
		mov ecx, esi
		call apply_spell_damage
		jmp[Perk44EarthPenalty_return]
	}
}

__declspec(naked) void Perk44EarthPenaltyAreaFork() {
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
		je PERK44_AREA_SKIP

		push dword ptr [esp + 0x1C]
		push ebp
		call DoApplyUnsettle
		add esp, 8

		PERK44_AREA_SKIP:
		pop eax
		pop edi
		pop esi
		pop ebx

		mov edx, ebp
		mov ecx, esi
		call apply_spell_damage
		jmp[Perk44EarthPenalty_area_return]
	}
}
