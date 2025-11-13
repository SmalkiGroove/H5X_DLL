#include "pch.h"

// Increase number of artifact sets to 40

void ArtfsetExpand_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00B5AD7A, 1, nullptr, 40, 0, 0, 0 });
}

// Change id of vanilla sets
/*
void ArtfsetRepoint_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB240, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB373, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x004BC55A, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x00B50538, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 8
	assembly_patches.push_back({ PATCH_BYTE, 0x0097B393, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 2
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4CD16, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 2*
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC1381, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 4
  //assembly_patches.push_back({ PATCH_BYTE, 0x008CF811, 1, nullptr, 33, 0, 0, 0 }); // Lions 2* -> spec Infantry Commander
	assembly_patches.push_back({ PATCH_BYTE, 0x00BBFC06, 1, nullptr, 33, 0, 0, 0 }); // Lions 3
  //assembly_patches.push_back({ PATCH_BYTE, 0x004BAB47, 1, nullptr, 34, 0, 0, 0 }); // Sar-issus 2 -> spec Shaman Matriarch
  //assembly_patches.push_back({ PATCH_INT,  0x008A3B02, 4, nullptr, 34, 0, 0, 0 }); // Sar-issus 2 -> spec Shaman Matriarch
	assembly_patches.push_back({ PATCH_BYTE, 0x00BBF8E8, 1, nullptr, 34, 0, 0, 0 }); // Sar-issus 4 & 2*
	assembly_patches.push_back({ PATCH_INT,  0x008A2064, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2
	assembly_patches.push_back({ PATCH_INT,  0x004E43C7, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x00989D0F, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x0098A1F3, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x009BC587, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_BYTE, 0x00C25EB8, 1, nullptr, 35, 0, 0, 0 }); // Necromancers 4
	assembly_patches.push_back({ PATCH_BYTE, 0x00C262C5, 1, nullptr, 35, 0, 0, 0 }); // Necromancers 4
	assembly_patches.push_back({ PATCH_INT,  0x008A8161, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 4
  //assembly_patches.push_back({ PATCH_BYTE, 0x00B4BBED, 1, nullptr, 36, 0, 0, 0 }); // Educational 2 -> spec Enlightened
  //assembly_patches.push_back({ PATCH_INT,  0x00A65C39, 4, nullptr, 37, 0, 0, 0 }); // Hunters 2 -> spec Tempress
  //assembly_patches.push_back({ PATCH_BYTE, 0x008CE90B, 1, nullptr, 37, 0, 0, 0 }); // Hunters 2* -> spec Lone Hunter
	assembly_patches.push_back({ PATCH_BYTE, 0x004BC576, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB25C, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2
	assembly_patches.push_back({ PATCH_BYTE, 0x008CE933, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2*
  //assembly_patches.push_back({ PATCH_INT,  0x004B0E55, 4, nullptr, 39, 0, 0, 0 }); // Runic 2* -> spec Elementalist
	assembly_patches.push_back({ PATCH_BYTE, 0x008603AA, 1, nullptr, 40, 0, 0, 0 }); // Demonic 2*
}
*/