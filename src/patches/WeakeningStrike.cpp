#include "pch.h"

// MOVE WEAKENING STRIKE EFFECT TO MASTER OF CURSES

void WeakeningStrike_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x008CF691, 1, nullptr, 46, 0, 0, 0 });
}
