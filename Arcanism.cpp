#include "pch.h"

// EDIT MAGIC PENETRATION VALUES FOR DUNGEON ARCANISM SKILL

void Arcanism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00F4531C, 1, nullptr, 25, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00F45320, 1, nullptr, 50, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00F45324, 1, nullptr, 75, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00F45328, 1, nullptr, 99, 0, 0, 0 });
}
