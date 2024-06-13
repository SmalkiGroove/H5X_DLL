#include "pch.h"

// ADD SPELL IMMUNITIES FROM ADDITIONAL ARTIFACTS
// (140) Sylvan Amulet : Immune to Curse of the Netherworld (21)
// (123) Obsidian Armor : Immune to Armageddon (10)
// (172) Sanctuary's Crown : Immune to Hypnotize (20)
// (165) Shantiri Breastplate : Immune to Earthquake (41)

void SpellImmuneFork();

void SpellImmune_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, 0x0097CB1F, 5, SpellImmuneFork, 0, 0, 0 });
}

__declspec(naked) void SpellImmuneFork() {
    __asm
    {
        
    }
}
