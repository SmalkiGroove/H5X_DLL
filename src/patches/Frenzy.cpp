#include "pch.h"

// FRENZY ALWAYS AFFECT FOR 1 ACTION

void Frenzy_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x0097D935, 1, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, 0x009BBFA9, 1, nullptr, 0, 0, 0, 0 });
}
