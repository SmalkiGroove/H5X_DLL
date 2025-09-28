#include "pch.h"

// REMOVE EFFECT OF MASTER OF ANIMATION SKILL

void MasterOfAnimationClear_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00BC0446, 3, nullptr, 0, 0, 0, 0 });
}
