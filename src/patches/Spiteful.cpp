#include "pch.h"

// Spiteful ability increases attack by +3 per friendly stack killed

void SpitefulTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x0098A5D5, 1, nullptr, 3, 0, 0, 0 });
}
