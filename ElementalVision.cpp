#include "pch.h"

// REMOVE HERO CLASS RESTRICTION FOR ELEMENTAL VISION SKILL

void ElementalVision_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x004B0F26, 2, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00B4B41E, 1, nullptr, 0, 0, 0, "EB"});
}

/*
0x00CDC42E check skill destro < 2 ?
0x00CDCD79 check skill destro >= 2 ?
0x00B4CDAB
*/