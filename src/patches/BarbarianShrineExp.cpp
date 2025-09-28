#include "pch.h"

// GAIN 500 EXP PER SHRINE SPELL LEVEL INSTEAD OF 1000

void BarbarianShrineExp_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00A81000, 2, nullptr, 0, 0, 0, 0 });
}
