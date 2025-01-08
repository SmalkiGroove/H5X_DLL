#include "pch.h"

// EDIT MAGIC PENETRATION VALUES FOR DUNGEON ARCANISM SKILL

void Arcanism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00F46F1C, 4, nullptr, 25, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00F46F20, 4, nullptr, 50, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00F46F24, 4, nullptr, 75, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00F46F28, 4, nullptr, 99, 0, 0, 0 });
}
