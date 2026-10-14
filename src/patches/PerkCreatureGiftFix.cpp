#include "pch.h"

// FIX: creatures given by perks join the matching stack of the army
// func_GetPerkGiftCreature (__fastcall(army, base creature), returns the creature to add, 0 = no room) picks the creature given by:
// - PERK_BATTLE_COMMANDER when learnt (func_ApplyStatFromSkill 0x00B54B85, base 45, Blade Dancer),
// - PERK_GOBLIN_SUPPORT every day (func_NewDayHero 0x00B5071E) and the other Goblin gifts (0x00B55888, 0x00B508ED;
//   base 117, Goblin).
// Vanilla: as soon as the army has a free slot it returns the first upgrade of the base creature (War Dancer, Goblin
// Trapper), even when a stack of the base creature or of its other upgrade is already there, which then fills a new
// slot. Only a full army makes it look for a matching stack (upgrades last-first, then the base).
// Fixed: a matching stack (same candidate order) is used first; otherwise a free slot gets the base creature.
// The whole function is replaced (same calling convention).

int __fastcall perk_creature_gift(int* army, int base);

int PerkCreatureGift_fork = 0x0045B8E0;    // func_GetPerkGiftCreature entry: sub esp,14h / push ebx / push esi

void PerkCreatureGiftFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PerkCreatureGift_fork, 5, (void*)perk_creature_gift, 0, 0, 0 });
}

// Army slots (army vtable +0x08): vector of ICreature*, 0 = free slot.
static int** army_slots(int* army, int*** end) {
	typedef int***(__thiscall* SlotsFn)(int*);
	int*** slots = ((SlotsFn)(*(int***)army)[0x08 / 4])(army);
	*end = (int**)slots[1];
	return (int**)slots[0];
}

// Slot holding a live stack of `creature` (same test as func_FindArmySlotOfCreature: the object's flag 0x80 marks a dead one).
static bool army_has(int* army, int creature) {
	int** end;
	for (int** slot = army_slots(army, &end); slot != end; ++slot) {
		char* stack = (char*)*slot;
		if (!stack) continue;
		char* object = stack + *(int*)(*(int*)(stack + 4) + 4);
		if (object[0xB] & 0x80) continue;
		if (((ICreature*)stack)->type_id == creature) return true;
	}
	return false;
}

static bool army_has_free_slot(int* army) {
	int** end;
	for (int** slot = army_slots(army, &end); slot != end; ++slot)
		if (!*slot) return true;
	return false;
}

int __fastcall perk_creature_gift(int* army, int base) {
	int* data = get_creature_data(base);
	if (!data) return base;
	int* upgrades = (int*)data[0x104 / 4];      // creature data +0x104: vector of upgrade IDs
	int* upgrades_end = (int*)data[0x108 / 4];
	if (upgrades == upgrades_end) return base;  // vanilla: no upgrade, the base creature

	for (int* upgrade = upgrades_end; upgrade != upgrades; )
		if (army_has(army, *--upgrade)) return *upgrade;
	if (army_has(army, base)) return base;
	return army_has_free_slot(army) ? base : 0;
}
