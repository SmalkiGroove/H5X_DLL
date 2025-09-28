#include "pch.h"

// EDIT MOVEMENT POINT MODIFIERS

void HeroMovement_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00CA9B79, 1, nullptr, 100, 0, 0, 0 }); // Movement points cost - Default
	assembly_patches.push_back({ PATCH_INT, 0x00CA9A7D, 4, nullptr, 90, 0, 0, 0 }); // Movement points cost - Homeland
	assembly_patches.push_back({ PATCH_INT, 0x00CA9AC7, 4, nullptr, 66, 0, 0, 0 }); // Movement points cost - Roads
	assembly_patches.push_back({ PATCH_INT, 0x00CA9B7D, 4, nullptr, 75, 0, 0, 0 }); // Movement points cost - Ship
	assembly_patches.push_back({ PATCH_INT, 0x00CA9AF1, 4, nullptr, 20, 0, 0, 0 }); // Movement points penalty - Dirt, Lava, Taiga, Sub
	assembly_patches.push_back({ PATCH_INT, 0x00CA9AFB, 4, nullptr, 20, 0, 0, 0 }); // Movement points penalty - Sand, Snow

	assembly_patches.push_back({ PATCH_WRTE, 0x00B4DF0C, 3, nullptr, 0, 0, 0, "F8E0B4" }); // Pointer to Logistics value
	assembly_patches.push_back({ PATCH_FLOAT, 0x00B4E0F8, 4, nullptr, 0, 0.05f, 0, 0 }); // Value of Logistics bonus
	assembly_patches.push_back({ PATCH_WRTE, 0x00B4DF47, 3, nullptr, 0, 0, 0, "F3E0B4" }); // Pointer to Boots of Swift Journey value
	assembly_patches.push_back({ PATCH_FLOAT, 0x00B4E0F3, 4, nullptr, 0, 0.15f, 0, 0 }); // Value of Boots of Swift Journey bonus
	assembly_patches.push_back({ PATCH_WRTE, 0x00B4DFC9, 6, nullptr, 0, 0, 0, "D80DBC39E200" }); // Pointer to Navigation value (25%)
}
