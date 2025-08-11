#include "pch.h"

// Hall of Intrigue dives knowledge to any hero class

void HallOfIntrigue_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00B4CDB4, 2, nullptr, 0, 0, 0, 0 });
}

