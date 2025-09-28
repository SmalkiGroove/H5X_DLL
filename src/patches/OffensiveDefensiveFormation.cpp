#include "pch.h"

// OFENSIVE AND DEFENSIVE FORMATION APPLIES TO ANY CREATURE

void OffDefFormation_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x008A250A, 1, nullptr, 0, 0, 0, "4C"});
	assembly_patches.push_back({ PATCH_NOP, 0x008A2512, 2, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x008A24F1, 6, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x008A296E, 6, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x008A29E6, 2, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x008A49EE, 6, nullptr, 0, 0, 0, 0 });
}
