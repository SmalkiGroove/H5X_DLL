#include "pch.h"

// ALWAYS ENABLE SIMULTANEOUS TURNS

void Pest_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x009C2660, 1, nullptr, 0, 0, 0, "C3" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0042E072, 2, nullptr, 0, 0, 0, "EB14" });
	assembly_patches.push_back({ PATCH_WRTE, 0x00865AE3, 2, nullptr, 0, 0, 0, "EB7F" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0098F909, 2, nullptr, 0, 0, 0, "EB34" });
	assembly_patches.push_back({ PATCH_WRTE, 0x009984F7, 2, nullptr, 0, 0, 0, "EB13" });
	assembly_patches.push_back({ PATCH_WRTE, 0x00B4B6D4, 2, nullptr, 0, 0, 0, "EB12" });
	assembly_patches.push_back({ PATCH_WRTE, 0x008659C5, 6, nullptr, 0, 0, 0, "E97005000090" });
	assembly_patches.push_back({ PATCH_NOP, 0x00AF3664, 5, nullptr, 0, 0, 0, 0 });
}
