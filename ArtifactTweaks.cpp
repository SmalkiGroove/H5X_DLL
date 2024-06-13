#include "pch.h"

// EDIT HARDCODED ARTIFACT EFFECTS


// Lower Staff of the Netherworld init debuff

void StaffOfNetherworldTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x008A4639, 4, nullptr, 0, 0, 0, "49478A00" });
	assembly_patches.push_back({ PATCH_FLOAT, 0x008A4749, 4, nullptr, 0, 0.1f, 0, 0 });
}


// Change Wand spells and charges

void WandOfSpellTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x00E92328, 4, nullptr, 3, 0, 0, 0 });   //  1 - Lightning Bolt
	assembly_patches.push_back({ PATCH_INT, 0x00E9232C, 4, nullptr, 4, 0, 0, 0 });   //  2 - Ice Bolt
	assembly_patches.push_back({ PATCH_INT, 0x00E92330, 4, nullptr, 5, 0, 0, 0 });   //  3 - Fireball
	assembly_patches.push_back({ PATCH_INT, 0x00E92334, 4, nullptr, 6, 0, 0, 0 });   //  4 - Frost Ring
	assembly_patches.push_back({ PATCH_INT, 0x00E92338, 4, nullptr, 8, 0, 0, 0 });   //  5 - Meteor Shower
	assembly_patches.push_back({ PATCH_INT, 0x00E9233C, 4, nullptr, 9, 0, 0, 0 });   //  6 - Implosion
	assembly_patches.push_back({ PATCH_INT, 0x00E92340, 4, nullptr, 38, 0, 0, 0 });  //  7 - Land Mines
	assembly_patches.push_back({ PATCH_INT, 0x00E92344, 4, nullptr, 236, 0, 0, 0 }); //  8 - Fire Wall
	assembly_patches.push_back({ PATCH_INT, 0x00E92348, 4, nullptr, 237, 0, 0, 0 }); //  9 - Stone Spikes
	assembly_patches.push_back({ PATCH_INT, 0x00E9234C, 4, nullptr, 279, 0, 0, 0 }); // 10 - Deep Freeze
	assembly_patches.push_back({ PATCH_INT, 0x00E92350, 4, nullptr, 12, 0, 0, 0 });  //  1 - 12 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92354, 4, nullptr, 12, 0, 0, 0 });  //  2 - 12 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92358, 4, nullptr, 8, 0, 0, 0 });   //  3 - 8 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E9235C, 4, nullptr, 8, 0, 0, 0 });   //  4 - 8 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92360, 4, nullptr, 2, 0, 0, 0 });   //  5 - 2 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92364, 4, nullptr, 2, 0, 0, 0 });   //  6 - 2 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92368, 4, nullptr, 12, 0, 0, 0 });  //  7 - 12 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E9236C, 4, nullptr, 4, 0, 0, 0 });   //  8 - 4 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92370, 4, nullptr, 8, 0, 0, 0 });   //  9 - 8 charges
	assembly_patches.push_back({ PATCH_INT, 0x00E92374, 4, nullptr, 2, 0, 0, 0 });   // 10 - 2 charges
}
