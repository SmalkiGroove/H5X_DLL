#include "pch.h"

// STACK OF N CREATURES ARE SPLITTED TO (1 | N-1) INSTEAD OF (N/2 | N/2)

void SplitStack_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x007866A6, 6, nullptr, 0, 0, 0, "B801000000C3" });
	assembly_patches.push_back({ PATCH_WRTE, 0x00786A3B, 8, nullptr, 0, 0, 0, "B801000000909090" });
}
