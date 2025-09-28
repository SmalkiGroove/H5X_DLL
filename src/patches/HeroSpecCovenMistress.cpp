#include "pch.h"

// Coven Mistress also applies on non-damaging and mass spells

void CovenMistressTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00978E30, 18, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00978F30, 3, nullptr, 0, 0, 0, "681E90" });
}
