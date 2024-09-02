#include "pch.h"

// Overheater has the effect of the Exploding Corpses skill

void OverheaterTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x0097B8D1, 10, nullptr, 0, 0, 0, "6A5389F1FF9294020000" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0097BB2C, 10, nullptr, 0, 0, 0, "6A5389F1FF9094020000" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0097BBE2, 1, nullptr, 0, 0, 0, "00" });
}
