#include "pch.h"

// EDIT THE AMOUNT OF BOUNCES OF CHAIN LIGHTNING AND CHAIN SHOT

void ChainHitCount_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00A5DD74, 1, nullptr, 2, 0, 0, 0 });
}
