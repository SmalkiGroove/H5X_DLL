#include "pch.h"

// Whip Strike spells changed

void WhipStrikeTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00A5E2DF, 4, nullptr, 1, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00A5E2E6, 4, nullptr, 13, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00A5E2F2, 4, nullptr, 17, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00A5E2E5, 1, nullptr, 0, 0, 0, "BB" });
	assembly_patches.push_back({ PATCH_WRTE, 0x00A5E2EA, 8, nullptr, 0, 0, 0, "83F84B7D02EB05BB" });
}
