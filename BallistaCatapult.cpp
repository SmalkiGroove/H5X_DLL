#include "pch.h"

// EDIT BALLISTA AND CATAPULT SHOTS

void BallistaCatapult_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00A75CB4, 1, nullptr, 99, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00A75CC0, 1, nullptr, 2, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00A75CE3, 1, nullptr, 2, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00A75D03, 1, nullptr, 0, 0, 0, 0 });
}
