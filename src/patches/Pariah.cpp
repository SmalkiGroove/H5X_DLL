#include "pch.h"

// PARIAH INCREASE SPELLPOWER BY 6

void PariahTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC04C8, 1, nullptr, 6, 0, 0, 0 });
}
