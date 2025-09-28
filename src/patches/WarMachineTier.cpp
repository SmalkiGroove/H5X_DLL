#include "pch.h"

// WM ARE NOW TIER 8

void WarMachineTier_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC1BA3, 1, nullptr, 8, 0, 0, 0 });
}
