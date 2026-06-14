#include "pch.h"

ThiscallInt_IntPtr_Int count_equipped_artifact = (ThiscallInt_IntPtr_Int)0x00CA6680;
ThiscallInt_IntPtr_Int count_backpack_artifact = (ThiscallInt_IntPtr_Int)0x00CA6B30;
ThiscallInt_IntPtr_Int skill_mastery = (ThiscallInt_IntPtr_Int)0x00B4D0B0;
ThiscallInt_IntPtr_Int has_skill = (ThiscallInt_IntPtr_Int)0x00BC32D0;

CdeclIntPtr_Void get_defaultstats = (CdeclIntPtr_Void)0x00BCB3F0;
ThiscallIntPtr_IntPtr get_hero_data = (ThiscallIntPtr_IntPtr)0x00977700;
ThiscallInt_IntPtr check_week_of_balance_luck = (ThiscallInt_IntPtr)0x00BCDD20;

ThiscallInt_SkillId get_skill_data = (ThiscallInt_SkillId)0x00875370;
ThiscallInt_SkillId is_skilltype_skill = (ThiscallInt_SkillId)0x00875380;
ThiscallInt_SkillId is_skilltype_perk = (ThiscallInt_SkillId)0x008753A0;
ThiscallInt_IntPtr get_skill_id = (ThiscallInt_IntPtr)0x009F5C90;
ThiscallInt_IntPtr_Int has_hero_spec = (ThiscallInt_IntPtr_Int)0x00B4B5A0;
ThiscallInt_IntPtr get_skill_mastery = (ThiscallInt_IntPtr)0x00ACFEF0;

ThiscallInt_IntPtr get_spell_id = (ThiscallInt_IntPtr)0x00A34110;
ThiscallInt_IntPtr get_spell_element = (ThiscallInt_IntPtr)0x00A342C0;
ThiscallInt_IntPtr get_spell_school = (ThiscallInt_IntPtr)0x00A34350;
ThiscallInt_IntPtr is_spell_empowered = (ThiscallInt_IntPtr)0x00A34100;
ThiscallInt_IntPtr get_unit_spellproof = (ThiscallInt_IntPtr)0x0097D2A0;

ThiscallIntPtr_Int get_creature_data = (ThiscallIntPtr_Int)0x0089FBF0;
FastcallByte_IntPtr func_GetCreatureFlying = (FastcallByte_IntPtr)0x004BA950;
ThiscallInt_Int_Int get_mitigated_damage = (ThiscallInt_Int_Int)0x0097D740;
ThiscallInt_IntPtr get_unit_luck_clamped = (ThiscallInt_IntPtr)0x004BB3C0;
FastcallUint_Int_IntPtr_Char get_damage_stack_count = (FastcallUint_Int_IntPtr_Char)0x00A6BD90;
ThiscallPtrPtr_Void construct_creature_lightning_fx_event = (ThiscallPtrPtr_Void)0x00C9A620;

StdcallPtr_Uint engine_alloc = (StdcallPtr_Uint)0x00878FD0;

FastcallVoid_IntPtr_Char_3Int notify_spec_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB930;
FastcallVoid_IntPtr_Char_3Int notify_skill_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB960;
FastcallVoid_IntPtr_Char_3Int notify_artifact_buff = (FastcallVoid_IntPtr_Char_3Int)0x009BB9C0;

FastcallVoid_Int enqueue_unit_refresh = (FastcallVoid_Int)0x0050C020;
