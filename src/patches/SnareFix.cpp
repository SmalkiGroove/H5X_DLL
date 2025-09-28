#include "pch.h"

// TOE bug fix: game crashing when Arcane Crystal or Blade Barrier are summoned on a tile with snare 

void SnareFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x009BB34A, 14, nullptr, 0, 0, 0, "8B168BE9578BCEFF526C85C07469" });
}