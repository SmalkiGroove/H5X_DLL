#include "pch.h"
#include "structs/CombatUnit.h"
#include "structs/Creature.h"
#include "types/Functions.h"

// PERK_SHOCKING_BOLTS (Shocking Bolts): lightning damage spells consume the target's retaliation.

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

	CombatUnit_vtable* caster_vt = *(CombatUnit_vtable**)caster_outer;
	if (!caster_vt->has_skill((int*)caster_outer, PERK_SHOCKING_BOLTS))
		return;

	CombatUnit_vtable* target_vt = *(CombatUnit_vtable**)target_outer;
	if (((ThiscallInt_IntPtr)target_vt->get_unit_kind)((int*)target_outer) != 1)
		return;

	int* counter = (int*)(target_outer - 0x80);
	if (*counter > 0) {
		(*counter)--;
		enqueue_unit_refresh(target_outer - 0x148);
	}
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
