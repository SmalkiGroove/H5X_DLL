#include "pch.h"

// SKIP CHECK IF PERK IS LEARNABLE

void GiveHeroSkill_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x005C46B6, 2, 0, 0, 0, 0 });
}
