#include "pch.h"

// CHANGE SKILL ID TO BE ABLE TO LEARN WARCRIES OR SPELLS

void LearnWarcries_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00B4B861, 4, nullptr, 92, 0, 0, 0 });
	//assembly_patches.push_back({ PATCH_INT, 0x00B4B964, 4, nullptr, 92, 0, 0, 0 });
}
