#include "pch.h"

// REMOVE BONUS MORALE FOR HERO OF SAME RACE

void BaseMorale_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x0045B563, 1, nullptr, 0, 0, 0, 0 });
}
