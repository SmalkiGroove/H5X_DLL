#include "pch.h"

// TWILIGHT INCREASE SPELLPOWER BY 4

void TwilightTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC049D, 1, nullptr, 4, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC04F8, 1, nullptr, 4, 0, 0, 0 });
}
