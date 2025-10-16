#include "pch.h"


// CHANGE SPEC ID 21 TO BUFF BLACK KNIGHTS

void SpecBlackKnights_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00F75694, 1, nullptr, 89, 0, 0, 0 });
}
