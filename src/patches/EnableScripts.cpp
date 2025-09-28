#include "pch.h"

// ALWAYS ENABLE LUA SCRIPTS ACTIVATION

void Scripts_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x0054CE31,  2, nullptr, 0, 0, 0, 0 }); // enable combat script
	assembly_patches.push_back({ PATCH_NOP, 0x00549CD2, 36, nullptr, 0, 0, 0, 0 }); // enable flyby ?
	assembly_patches.push_back({ PATCH_NOP, 0x00607C3A,  6, nullptr, 0, 0, 0, 0 }); // enable script debugging
	assembly_patches.push_back({ PATCH_NOP, 0x00608ED0,  6, nullptr, 0, 0, 0, 0 }); // enable advmap script
	assembly_patches.push_back({ PATCH_NOP, 0x0061D387,  2, nullptr, 0, 0, 0, 0 }); // enable town script
}
