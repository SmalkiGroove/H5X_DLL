#include "pch.h"

// Master of Element no longer summon Elementals on friendly creature death

void MasterOfElementsTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x00A5A60E, 6, nullptr, 0, 0, 0, "E91D01000090" });
}
