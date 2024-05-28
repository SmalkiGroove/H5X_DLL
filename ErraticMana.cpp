#include "pch.h"

// CHANGE ERRATIC MANA BEHAVIOUR

void ErraticMana_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4C543, 1, nullptr, 100, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00B4C56D, 4, nullptr, 0, 0, 0, "9856E100" });
	assembly_patches.push_back({ PATCH_BYTE, 0x00BBF6DB, 1, nullptr, 100, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00BBF712, 4, nullptr, 0, 0.90f, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00BBF717, 4, nullptr, 0, 0.75f, 0, 0 });
}
