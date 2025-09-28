#include "pch.h"
// Disables usage of Rune of Dragonform for Fire Dragon.

void RuneOfTheDragonForm_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x004BB013, 11, nullptr, 0, 0, 0, "8B808C00000083F8077C15" });
    assembly_patches.push_back({ PATCH_NOP, 0x004BB01E, 17, nullptr, 0, 0, 0, 0 });
}