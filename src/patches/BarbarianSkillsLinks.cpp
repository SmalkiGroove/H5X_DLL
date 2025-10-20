#include "pch.h"

// CHANGE BARBARIAN SKILLS MAPPING

void BarbarianSkillsLinks_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, 0x0087547A, 4, nullptr, SKILL_BLOOD_RAGE, 0, 0, 0 });      //183 - RAGE_AWAKENING
	assembly_patches.push_back({ PATCH_INT, 0x0087542C, 4, nullptr, PERK_BLOCK_CORRUPTION, 0, 0, 0 }); //208 - BLOCK_CORRUPTION
	assembly_patches.push_back({ PATCH_INT, 0x00875432, 4, nullptr, PERK_ARMOR_PROTECTION, 0, 0, 0 }); //209 - ARMOR_PROTECTION
	assembly_patches.push_back({ PATCH_INT, 0x00875438, 4, nullptr, PERK_SOIL_BURN, 0, 0, 0 });        //210 - SCORCHING EARTH
	assembly_patches.push_back({ PATCH_INT, 0x0087543E, 4, nullptr, SKILL_DESPOTISM, 0, 0, 0 });       //211 - DESPOTISM
	assembly_patches.push_back({ PATCH_INT, 0x00875444, 4, nullptr, PERK_DEVOTION, 0, 0, 0 });         //212 - DEVOTION
	assembly_patches.push_back({ PATCH_INT, 0x0087544A, 4, nullptr, PERK_BATTLE_WRATH, 0, 0, 0 });     //213 - BATTLE_WRATH
	assembly_patches.push_back({ PATCH_INT, 0x00875450, 4, nullptr, PERK_WAR_POLICY, 0, 0, 0 });       //214 - WAR_POLICY
	assembly_patches.push_back({ PATCH_INT, 0x00875456, 4, nullptr, PERK_TELEPORT_ASSAULT, 0, 0, 0 }); //215 - INFERNAL_PORTALS
	assembly_patches.push_back({ PATCH_INT, 0x0087545C, 4, nullptr, PERK_BURNING_BLOOD, 0, 0, 0 });    //216 - FIRE_RESISTANCE
	assembly_patches.push_back({ PATCH_INT, 0x00875462, 4, nullptr, PERK_ABSOLUTE_MASTERY, 0, 0, 0 }); //217 - ADVANCED_BLOOD_RAGE
	assembly_patches.push_back({ PATCH_INT, 0x00875468, 4, nullptr, PERK_EXHAUST, 0, 0, 0 });          //218 - EXHAUST
	assembly_patches.push_back({ PATCH_INT, 0x0087546E, 4, nullptr, PERK_SPELLBREAKER, 0, 0, 0 });     //219 - SPELLBREAKER
	assembly_patches.push_back({ PATCH_INT, 0x00875474, 4, nullptr, PERK_MENTORING, 0, 0, 0 });        //220 - TALENTED_WARRIOR
}
