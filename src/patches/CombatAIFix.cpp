#include "pch.h"

//  AI will become more active in using mass and other abilities and spells that do not require a target 
//(like summoning a phoenix and elementals), as well as more adequately assessing the power of the Deflect Missile spell.
// by RedHeavenHero https://forum.heroesworld.ru/showthread.php?t=15624

void CombatAIFix();

void CombatAIFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_NOP, 0x00BFBD90, 4, nullptr, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_NOP, 0x00BFC0F0, 8, nullptr, 0, 0, 0, 0 });  // counterspell related
    assembly_patches.push_back({ PATCH_WRTE, 0x008D757E, 1, nullptr, 0, 0, 0, "00" });
}