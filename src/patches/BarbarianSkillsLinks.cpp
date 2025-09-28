#include "pch.h"

// CHANGE BARBARIAN SKILLS MAPPING

void BarbarianSkillsLinks_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x0087547A, 4, nullptr, 183, 0, 0, 0 }); //183
	assembly_patches.push_back({ PATCH_INT, 0x0087542C, 4, nullptr, 208, 0, 0, 0 }); //208
	assembly_patches.push_back({ PATCH_INT, 0x00875432, 4, nullptr, 209, 0, 0, 0 }); //209
	//assembly_patches.push_back({ PATCH_INT, 0x00875438, 4, nullptr, 210, 0, 0, 0 }); //210 - SCORCHING_EARTH
	assembly_patches.push_back({ PATCH_INT, 0x0087543E, 4, nullptr, 211, 0, 0, 0 }); //211
	assembly_patches.push_back({ PATCH_INT, 0x00875444, 4, nullptr, 212, 0, 0, 0 }); //212 - INTELLIGENCE
	assembly_patches.push_back({ PATCH_INT, 0x0087544A, 4, nullptr, 213, 0, 0, 0 }); //213
	assembly_patches.push_back({ PATCH_INT, 0x00875450, 4, nullptr, 214, 0, 0, 0 }); //214
	assembly_patches.push_back({ PATCH_INT, 0x00875456, 4, nullptr, 215, 0, 0, 0 }); //215
	//assembly_patches.push_back({ PATCH_INT, 0x0087545C, 4, nullptr, 216, 0, 0, 0 }); //216 - FIRE_RESISTANCE
	//assembly_patches.push_back({ PATCH_INT, 0x00875462, 4, nullptr, 217, 0, 0, 0 }); //217 - MANA_BURST
	//assembly_patches.push_back({ PATCH_INT, 0x00875468, 4, nullptr, 218, 0, 0, 0 }); //218 - EXHAUST
	assembly_patches.push_back({ PATCH_INT, 0x0087546E, 4, nullptr, 219, 0, 0, 0 }); //219
	//assembly_patches.push_back({ PATCH_INT, 0x00875474, 4, nullptr, 220, 0, 0, 0 }); //220 - TALENTED_WARRIOR
}
