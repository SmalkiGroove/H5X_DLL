#include "pch.h"

// SUMMON ELEMENTALS AND CONJURE PHOENIX SET CREATURE ATB

void ConjureAtb_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT, 0x00BDA55E, 4, nullptr, 0, constf_0_5, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00BDA55E, 4, nullptr, 0, constf_0_5, 0, 0 });
}
