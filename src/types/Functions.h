#pragma once

#include "Artifacts.h"
#include "Skills.h"
#include "structs/Hero.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

typedef int(__thiscall* CountEquippedArtifacts)(int*, ARTIFACT_ID);
extern CountEquippedArtifacts count_equipped_artifact;

typedef int(__thiscall* CountBackpackArtifacts)(int*, ARTIFACT_ID);
extern CountBackpackArtifacts count_backpack_artifact;

typedef int(__thiscall* GetSkillData)(SKILL_ID);
extern GetSkillData get_skill_data;

typedef int(__thiscall* IsSkilltypeSkill)(SKILL_ID);
extern IsSkilltypeSkill is_skilltype_skill;

typedef int(__thiscall* IsSkilltypePerk)(SKILL_ID);
extern IsSkilltypePerk is_skilltype_perk;

typedef int(__thiscall* SkillMasteryLevel)(int*, SKILL_ID);
extern SkillMasteryLevel skill_mastery;


#endif
