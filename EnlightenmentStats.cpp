#include "pch.h"

// REMOVE STATS GAINS FROM LEARNING SKILL

void EnlightenmentStats_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00B4BEAA, 4, nullptr, 255, 0, 0, 0 });
}
