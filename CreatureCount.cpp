#include "pch.h"

// EXPAND CREATURES SLOTS FROM 180 TO 200

void CreatureCount_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x006CA781, 4, nullptr, 200, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x006E1A20, 4, nullptr, 200, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x0000E076, 2, nullptr, 0, 0, 0, "EB3B"});
	assembly_patches.push_back({ PATCH_WRTE, 0x0000E0B3, 9, nullptr, 0, 0, 0, "83FF0C74F489CEEBBC"});
}
