#include "pch.h"

// CREATURE SPELLPOWER SCALE LINEARLY WITH STACK SIZE

void CreatureSpellpower_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x004BAD36, 23, nullptr, 0, 0, 0, "8D04B601C099F7B9A8000000C1E80283C0015E83C408C3" });
	assembly_patches.push_back({ PATCH_NOP, 0x004BAD4A, 71, nullptr, 0, 0, 0, 0 });
}
