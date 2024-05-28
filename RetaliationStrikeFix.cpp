#include "pch.h"

// RETALIATION STRIKE ALSO APPLIES WHEN CREATURE IS ATTACKED BY A SHOOTER

void RetaliationStrikeFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00A6556B, 2, nullptr, 0, 0, 0, 0 });
}
