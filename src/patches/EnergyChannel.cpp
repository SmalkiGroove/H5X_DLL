#include "pch.h"

// Energy Channel reduce hero mana cost from 25% to 10%

void EnergyChannelTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097828C, 4, nullptr, 0, constf_0_9, 0, 0 });
}
