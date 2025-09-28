#include "pch.h"

// MERGE "SKILLS" AND "BIOGRAPHY" PANES INTO ONE

void HeroScreenUI_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x006CA3F4, 9, nullptr, 0, 0, 0, "688DFFE1006884FFE1" });
	assembly_patches.push_back({ PATCH_WRTE, 0x006CA41C, 9, nullptr, 0, 0, 0, "8D8EF8000000E809FB" });
	assembly_patches.push_back({ PATCH_WRTE, 0x006CA434, 9, nullptr, 0, 0, 0, "686652E300686052E3" });
	assembly_patches.push_back({ PATCH_WRTE, 0x006CA464, 9, nullptr, 0, 0, 0, "8D8EC4000000E801D6" });
	assembly_patches.push_back({ PATCH_WRTE, 0x006CA46F, 2, nullptr, 0, 0, 0, "EB2A" });
}
