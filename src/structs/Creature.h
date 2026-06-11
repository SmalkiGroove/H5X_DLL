#pragma once
#ifndef CREATURE_H
#define CREATURE_H

#include "structs/CombatUnit.h"

// CombatCreature (vtable 0x00E4FBA4, NWorld::CCombatCreature, vtordisp -0x144).
// RE rule: .cursor/rules/h5x-re-combat-creature.mdc (auto-synced).
//
// Subobject on a combat unit (from unit pointer in esi/ebp):
//   mov eax, [unit - 0x144]          ; vbtable
//   mov ecx, [eax + 8]
//   lea ecx, [ecx + unit - 0x144]    ; ICombatCreature*
// Creature type id on the outer unit: [unit + 0x1C].

struct ICombatCreature : ICombatUnit {
	int* creature_ref() {
		return instance->get_unit_ref((int*)this);
	}
	int mana_points() {
		return instance->get_mana((int*)this);
	}
	bool has_ability(int ability_id) {
		return instance->has_ability((int*)this, ability_id) != 0;
	}
};

#endif
