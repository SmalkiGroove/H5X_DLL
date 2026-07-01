#pragma once

#include "types/types.h"

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

// --- Hero / inventory ---

extern ThiscallInt_IntPtr_Int count_equipped_artifact;
extern ThiscallInt_IntPtr_Int count_backpack_artifact;
extern ThiscallInt_IntPtr_Int skill_mastery;
extern ThiscallInt_IntPtr_Int has_skill;
extern CdeclIntPtr_Void get_defaultstats;
extern ThiscallIntPtr_IntPtr get_hero_data;
extern ThiscallInt_IntPtr check_week_of_balance_luck;

// --- Skills / specs ---

extern ThiscallInt_SkillId get_skill_data;
extern ThiscallInt_SkillId is_skilltype_skill;
extern ThiscallInt_SkillId is_skilltype_perk;
extern ThiscallInt_IntPtr get_skill_id;
extern ThiscallInt_IntPtr_Int has_hero_spec;
extern ThiscallInt_IntPtr get_skill_mastery;

// --- Spells ---

extern ThiscallInt_IntPtr get_spell_id;
extern ThiscallInt_IntPtr get_spell_element;
extern ThiscallInt_IntPtr get_spell_school;
extern ThiscallInt_IntPtr is_spell_empowered;
extern ThiscallInt_IntPtr get_unit_spellproof;
extern FastcallInt_IntPtr_IntPtr_Int_Int_Int_Int apply_spell_damage;

// --- Creatures / combat ---

extern ThiscallIntPtr_Int get_creature_data;
extern FastcallByte_IntPtr func_GetCreatureFlying;
extern ThiscallInt_Int_Int get_mitigated_damage;
extern ThiscallInt_IntPtr get_unit_luck_clamped;
extern FastcallUint_Int_IntPtr_Char get_damage_stack_count;
extern ThiscallPtrPtr_Void construct_creature_lightning_fx_event;

// --- Engine alloc ---

extern StdcallPtr_Uint engine_alloc;

// --- Buff notification (engine UI hooks) ---

extern FastcallVoid_IntPtr_Char_3Int notify_skill_buff;
extern FastcallVoid_IntPtr_Char_3Int notify_artifact_buff;
extern FastcallVoid_IntPtr_Char_3Int notify_spec_buff;

// --- CRM / unit refresh ---

extern FastcallVoid_Int enqueue_unit_refresh;

#endif
