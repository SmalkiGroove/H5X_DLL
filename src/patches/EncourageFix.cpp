#include "pch.h"

// Fix: Encourage should not be affected by magic resistance

void EncourageFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00A34158, 1, nullptr, 1, 0, 0, 0 });
}
