#include "pch.h"
#include "types/Abilities.h"
#include "types/Functions.h"

// Magnetic Field (ABILITY_MAGNETIC_FIELD = 148): when an adjacent ally is targeted by a
// ranged attack, a nearby stack with this ability redirects the shot to itself (100%).
//
// Hook in func_CombatAttackWithRedirects @ 0x00A65D50, at 0x00A65EB3 (all paths converge
// here after the Taunt loop). EBX = current target, ESI = attacker.

void MagnetismFork();

int Magnetism_fork = 0x00A65EB3;
int Magnetism_continue = 0x00A65EBA;

static ICombatUnit* __cdecl FindMagneticRedirectTarget(
	ICombatUnit* attacker,
	ICombatUnit* target,
	int param8,
	int param9
) {
	if (!attacker || !target || !attacker->instance || !target->instance)
		return target;

	int adjacent_units_vec[3] = { 0, 0, 0 };
	int& list_begin = adjacent_units_vec[0];
	int& list_end = adjacent_units_vec[1];

	// Build the same temporary unit list as func_CombatAttackWithRedirects.
	collect_adjacent_units((int*)attacker, adjacent_units_vec, param8, param9, 1, 1, 0);

	// Pull the same stats field used by vanilla redirect checks.
	int* default_stats = get_defaultstats();

	int target_side = ((GetInt)target->instance->is_side_zero)((int*)target);
	ICombatUnit* redirected_target = target;

	if (list_begin && list_begin < list_end && default_stats) {
		const int adjacency_roll_arg = *(int*)((char*)default_stats + 0x52C);
		for (int** it = (int**)list_begin; it < (int**)list_end; ++it) {
			ICombatUnit* candidate = (ICombatUnit*)(*it);
			if (!candidate || !candidate->instance || candidate == target)
				continue;

			if (candidate->instance->has_ability((int*)candidate, ABILITY_MAGNETIC_FIELD) == 0)
				continue;

			int candidate_side = ((GetInt)candidate->instance->is_side_zero)((int*)candidate);
			if (candidate_side != target_side)
				continue;

			// Reuse vanilla adjacency computation (same helper used in Taunt path).
			int adjacency_roll =
				get_unit_adjacency_roll((int*)candidate, (int*)target, 1, adjacency_roll_arg);

			if (adjacency_roll > 0) {
				redirected_target = candidate;
				break;
			}
		}
	}

	if (list_begin) {
		engine_free((void*)list_begin);
	}

	return redirected_target;
}

void Magnetism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Magnetism_fork, 7, MagnetismFork, 0, 0, 0 });
}

__declspec(naked) void MagnetismFork() {
	__asm {
		// param_5 ([esp+0xA4]) is non-zero for ranged attacks.
		cmp byte ptr [esp + 0xA4], 0x0
		je MAGNETISM_CONTINUE_ORIGINAL

		mov edx, dword ptr [esp + 0xB0] // param_8
		mov ecx, dword ptr [esp + 0xB4] // param_9
		push ecx
		push edx
		push ebx
		push esi
		call FindMagneticRedirectTarget
		add esp, 0x10
		mov ebx, eax

		MAGNETISM_CONTINUE_ORIGINAL:
		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax + 0x6C]
		jmp[Magnetism_continue]
	}
}
