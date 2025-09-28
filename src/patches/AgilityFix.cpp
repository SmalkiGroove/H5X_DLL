#include "pch.h"

// Fix: "Agility" ability begins chargeed at the start of combat even though the creatures have not moved yet.

void AgilityFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x008AB7EF, 1, nullptr, 0, 0, 0, "EB" });
}
