#include "pch.h"

// All seeing crown gives +127 vision range instead of +4

void AllSeeingCrownTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4C020, 1, nullptr, 127, 0, 0, 0 });
}
