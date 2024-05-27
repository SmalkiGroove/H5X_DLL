#include "pch.h"

// MAKE COMBAT ARENAS BIGGER

int Battlefield_width = 14;
int Battlefield_height = 11;

void BattlefieldSize_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00AF14B6, 4, nullptr, Battlefield_width, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00AF14BC, 4, nullptr, Battlefield_height, 0, 0, 0 });
}
