#include "pch.h"

// ALWAYS ENABLE LUA SCRIPTS ACTIVATION

void Scripts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x0014C231,  2, nullptr, 0, 0, 0, 0 }); // enable combat script
	assembly_patches.push_back({ PATCH_NOP, 0x001490D2, 36, nullptr, 0, 0, 0, 0 }); // enable flyby ?
	assembly_patches.push_back({ PATCH_NOP, 0x0020703A,  6, nullptr, 0, 0, 0, 0 }); // enable script debugging
	assembly_patches.push_back({ PATCH_NOP, 0x002082D0,  6, nullptr, 0, 0, 0, 0 }); // enable advmap script
	assembly_patches.push_back({ PATCH_NOP, 0x0021C787,  2, nullptr, 0, 0, 0, 0 }); // enable town script
}
