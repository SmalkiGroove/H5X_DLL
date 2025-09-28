#include "pch.h"

// CHANGE DEFAULT ATB VALUES

float CreatureMaxStartAtb = 0.05f;
float WaitActionAtbReset = 0.50f;
float MassSpellAtbReset = 0.00f;
float SorcerySpellAtbReset = 0.05f;

void AtbTweaks_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT, 0x00A740F7, 4, nullptr, 0, CreatureMaxStartAtb, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00A6B59C, 4, nullptr, 0, WaitActionAtbReset, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00BBF87F, 4, nullptr, 0, MassSpellAtbReset, 0, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, 0x00400F3C, 4, nullptr, 0, SorcerySpellAtbReset, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00BBF8C9, 4, nullptr, 0, 0, 0, "3C034000" });
}
