#include "pch.h"

// PERK_SHOCKING_BOLTS : lightning damage spells consume the target's retaliation and stun twice as much.

void ShockingBoltsFork();
void ShockingBoltsStunFork();

int ShockingBolts_fork = 0x004E6B19;       // spell damage, stun branch (lightning): push esi / mov edx,ebp / mov ecx,edi
int ShockingBolts_return = 0x004E6B1E;     // call 0x004E5C20 (lightning stun)
int ShockingBoltsStun_fork = 0x004E5C7C;   // lightning stun (0x004E5C20): fld [esp+8] (ATB) / fmul [eax+7A0h] (DefaultStats stun factor)
int ShockingBoltsStun_return = 0x004E5C86;

void ShockingBolts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShockingBolts_fork, 5, ShockingBoltsFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ShockingBoltsStun_fork, 10, ShockingBoltsStunFork, 0, 0, 0 });
}

static bool has_shocking_bolts(ICombatUnit* caster) {
	return caster && caster->unit_kind() == 2 && ((ICombatHero*)caster)->has_perk(PERK_SHOCKING_BOLTS);
}

// caster / target = CombatUnit subobjects. Removes one retaliation from a creature target.
static void __fastcall shocking_bolts_retaliation(ICombatUnit* caster, ICombatUnit* target) {
	if (!target || !has_shocking_bolts(caster)) return;
	if (target->unit_kind() != 1) return;

	int* retaliations_left = (int*)((char*)target - 0x80);
	if (*retaliations_left > 0) {
		(*retaliations_left)--;
		enqueue_unit_refresh((int)target - CombatCreature_subobject_offset);
	}
}

// Fraction of the ATB removed by the lightning stun (vanilla DefaultStats +0x7A0), doubled by the perk.
static float __fastcall lightning_stun_factor(ICombatUnit* caster) {
	float factor = *(float*)((char*)get_defaultstats() + 0x7A0);
	if (has_shocking_bolts(caster))
		factor *= 2.0f;
	return factor;
}

// edi = caster, ebp = target; replays the arguments of the stun call
__declspec(naked) void ShockingBoltsFork() {
	__asm {
		mov ecx, edi
		mov edx, ebp
		call shocking_bolts_retaliation

		push esi
		mov edx, ebp
		mov ecx, edi
		jmp [ShockingBolts_return]
	}
}

// edi = caster, ATB at [esp+8]; leaves st0 = ATB * factor like the replaced fld / fmul
__declspec(naked) void ShockingBoltsStunFork() {
	__asm {
		mov ecx, edi
		call lightning_stun_factor
		fmul dword ptr [esp + 0x8]
		jmp [ShockingBoltsStun_return]
	}
}
