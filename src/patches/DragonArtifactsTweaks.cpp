#include "pch.h"

// Change initiative bonus for Dragon artifacts to 5%

void DragonArtifactTweaks_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x004BC35D, 6, nullptr, 0, 0, 0, "8D0C308D3481"}); // Dragon Bone Greaves
	assembly_patches.push_back({ PATCH_WRTE, 0x004BC37A, 6, nullptr, 0, 0, 0, "8D0C308D3481" }); // Dragon Wing Mantle
	assembly_patches.push_back({ PATCH_WRTE, 0x004BC3AB, 6, nullptr, 0, 0, 0, "8D0C308D3481" }); // Dragon Teeth Necklace
	assembly_patches.push_back({ PATCH_WRTE, 0x004BC3C8, 6, nullptr, 0, 0, 0, "8D0C308D3481" }); // Dragon Eye Ring
}
