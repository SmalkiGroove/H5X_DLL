#include "pch.h"

// REMOVE HERO CLASS RESTRICTION FOR AVENGER SKILL

void Avengers_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00B594CE, 2, nullptr, 0, 0, 0, 0 });
}
