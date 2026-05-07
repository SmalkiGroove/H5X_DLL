#include "pch.h"

// Spiteful ability increases attack by +3 per friendly stack killed

void SpitefulTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x008A3D1A, 1, nullptr, 3, 0, 0, 0 });
}
