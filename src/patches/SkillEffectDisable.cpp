#include "pch.h"

// REMOVE HARDCODED SKILL EFFECTS

void SkillEffectDisable_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00BC0446, 3, nullptr, 0, 0, 0, 0 }); // Master of Life
	assembly_patches.push_back({ PATCH_WRTE, 0x009782E2, 1, nullptr, 0, 0, 0, "EB" }); // Suppress Light/Dark
	//assembly_patches.push_back({ PATCH_WRTE, 0x0097B054, 1, nullptr, 0, 0, 0, "EB" }); // Luck of the Barbarian

}
