#include "pch.h"

// CHAIN LIGHTNING BOUNCES DEAL 75% OF PREVIOUS BOUNCE DAMAGES INSTEAD OF 50%

void ChainLightning_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x0085F00A, 3, nullptr, 0, 0, 0, "5C4CE3" });
}
