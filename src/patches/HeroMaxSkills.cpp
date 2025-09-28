#include "pch.h"

// HEROES CAN LEARN UP TO 8 SKILLS

void HeroMaxSkills_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x00B50DE5, 1, nullptr, 0, 0, 0, "1C" });
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4D399, 1, nullptr, 9, 0, 0, 0 });
}
