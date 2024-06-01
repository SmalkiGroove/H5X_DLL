#include "pch.h"

// EARTHQUAKE IS ALWAYS AFFECTED BY TREMORS PERK

void Earthquake_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x0097C38C, 6, nullptr, 0, 0, 0, "E99200000090" });
	assembly_patches.push_back({ PATCH_NOP, 0x0097E524, 6, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x0097E5FB, 6, nullptr, 0, 0, 0, 0 });
}
