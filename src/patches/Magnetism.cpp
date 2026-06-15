#include "pch.h"
#include "types/Abilities.h"
#include "types/Functions.h"

// Magnetism (ABILITY_MAGNETIC_FIELD): when an adjacent ally is targeted by a
// ranged attack, a nearby stack with this ability redirects the shot to itself (100%).
//
// Hook in func_CombatAttackWithRedirects @ 0x00A65D50, at 0x00A65EB3 (all paths converge
// here after the Taunt loop). EBX = current target, ESI = attacker.

namespace {

using CollectAdjacentUnitsFn = void(__fastcall*)(
	int* refUnit, int* outVec, int tileX, int tileY, char param5, char param6);
using EngineFreeFn = void(__stdcall*)(void*);
// Returns true when tile is within the attacker's melee reach (used by multi-shot bounce).
using IsTileInMeleeRangeFn = char(__fastcall*)(int* unit, int size, int tileX, int tileY);

CollectAdjacentUnitsFn collect_adjacent_units = (CollectAdjacentUnitsFn)0x00B3D560;
EngineFreeFn engine_free = (EngineFreeFn)0x00879240;
IsTileInMeleeRangeFn is_tile_in_melee_range = (IsTileInMeleeRangeFn)0x00B3C980;

static bool attacker_in_melee_range(int* attacker, int tileX, int tileY) {
	if (!attacker)
		return true;
	auto* vt = *(void***)attacker;
	auto getSize = (int(__thiscall*)(int*))vt[0x44 / 4];
	int size = getSize(attacker);
	return is_tile_in_melee_range(attacker, size, tileX, tileY) != 0;
}

static bool unit_has_ability(int* unit, int abilityId) {
	if (!unit)
		return false;
	auto* vt = *(void***)unit;
	auto hasAbility = (HasCreatureAbility)vt[0x28C / 4];
	return hasAbility(unit, abilityId) != 0;
}

static void collect_adjacent_allies(int* refUnit, int tileX, int tileY, int outVec[3]) {
	outVec[0] = 0;
	outVec[1] = 0;
	outVec[2] = 0;
	collect_adjacent_units(refUnit, outVec, tileX, tileY, 1, 0);
}

} // namespace

static int* __cdecl magnetism_try_redirect(
	int* attacker,
	int* target,
	int attackerTile,
	int tileX,
	int tileY,
	char skipRedirect,
	int effectKind)
{
	(void)attackerTile;

	if (!attacker || !target || skipRedirect != 0)
		return target;
	if (effectKind == 0x9B || effectKind == 0x15B)
		return target;
	// param_10 is not a reliable isRanged flag (main path passes 0 for ranged shots).
	// Use the engine melee-reach test instead of inverting param_10.
	if (attacker_in_melee_range(attacker, tileX, tileY))
		return target;

	int vec[3] = {};
	collect_adjacent_allies(target, tileX, tileY, vec);

	int* begin = (int*)vec[0];
	int* end = (int*)vec[1];
	int* redirected = target;

	if (begin && end > begin) {
		int count = (int)(end - begin);
		for (int i = 0; i < count; ++i) {
			int* candidate = ((int**)begin)[i];
			if (!candidate || candidate == target)
				continue;
			if (!unit_has_ability(candidate, ABILITY_MAGNETIC_FIELD))
				continue;

			redirected = candidate;
			break;
		}
	}

	if (begin)
		engine_free(begin);
	return redirected;
}

void MagnetismFork();

// Replaces 7 bytes @ 0x00A65EB3:
//   mov eax,[esi]          ; 2
//   mov ecx,esi            ; 2
//   call dword ptr [eax+6C]; 3  (FF 50 6C — disp8, not disp32)
int Magnetism_fork = 0x00A65EB3;
int Magnetism_continue = 0x00A65EBA;

void Magnetism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Magnetism_fork, 7, MagnetismFork, 0, 0, 0 });
}

__declspec(naked) void MagnetismFork() {
	__asm {
		push esi
		push edi
		lea eax, [esp + 8] // frame base E at hook entry

		push dword ptr [eax + 0xAC] // param_7 (effect kind)
		push dword ptr [eax + 0xA4] // param_5 (skip taunt when non-zero)
		push dword ptr [eax + 0xB4] // param_9
		push dword ptr [eax + 0xB0] // param_8
		push dword ptr [eax + 0x14] // attacker tile
		push ebx                    // target
		push dword ptr [esp + 0x20] // attacker (saved esi)
		call magnetism_try_redirect
		add esp, 0x1C
		mov ebx, eax

		pop edi
		pop esi

		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax + 0x6C]
		jmp[Magnetism_continue]
	}
}
