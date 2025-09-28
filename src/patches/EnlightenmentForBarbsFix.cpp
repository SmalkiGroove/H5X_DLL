#include "pch.h"

// Fixed bug - unlearning Barbarian Learning does not revoke primary skill bonuses

void EnlightenmentForBarbsFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x00B56040, 1, nullptr, 0, 0, 0 });
}