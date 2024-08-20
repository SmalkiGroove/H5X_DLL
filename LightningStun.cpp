#include "pch.h"

// LIGHTNING SPELLS ARE ALWAYS AFFECTED BY MASTER OF LIGHTNING PERK

void LightningStun_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x004E5C29, 6, nullptr, 0, 0, 0, "E91D00000090" });
}
