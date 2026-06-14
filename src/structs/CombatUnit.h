#pragma once
#ifndef COMBAT_UNIT_H
#define COMBAT_UNIT_H

// Shared MSVC virtual interface for in-combat fighters (NWorld::CCombatHero, NWorld::CCombatCreature).
// Parallel vtables with identical slot layout (offsets 0..0x2A8); 92/171 slots share the same impl.
// RE rule: .cursor/rules/h5x-re-combat-unit.mdc (auto-synced).
//
// Vtable instances:
//   CCombatHero     @ 0x00E8499C, vtordisp -0x118 (Ghidra: struct_CombatHero in func_InitCombat)
//   CCombatCreature @ 0x00E4FBA4, vtordisp -0x144 (inherits CTimedStats@NWorld per RTTI)
//
// Thunks: sub ecx,[ecx-4]; sub ecx,<vtordisp>; jmp <impl>. Hero vtordisp 0x68, creature 0x94.
// Slots tagged [shared] resolve to the same function in both vtables (see func_CombatUnit_* in Ghidra).

#include "types/types.h"

enum {
	CombatHero_vtable_addr = 0x00E8499C,
	CombatCreature_vtable_addr = 0x00E4FBA4,
	CombatHero_vtordisp = 0x118,
	CombatCreature_vtordisp = 0x144
};

struct CombatUnit_vtable {
	int* get_unit_kind; // 0x000 (0) - hero 0x00A83AA0 func_CombatHero_GetUnitKind (ret 2); creature 0x008B1E10 func_CombatCreature_GetUnitKind (ret 1)
	int* scalar_deleting_dtor; // 0x004 (4) - 0x008AEDC0 [shared] func_CombatUnit_ScalarDeletingDtor
	int* get_owner_ref; // 0x008 (8) - 0x008ACDC0 [shared] func_CombatUnit_GetOwnerRef
	GetUnitRef get_unit_ref; // 0x00C (12) - 0x008AEAF0 [shared] func_CombatUnit_GetUnitRef
	int* notify_owner; // 0x010 (16) - 0x00A6B0E0 [shared] func_CombatUnit_NotifyOwner
	int* init_owner_timed_stats; // 0x014 (20) - hero 0x008B1780 func_CombatHero_InitOwnerTimedStats; creature 0x008A0950 func_CombatCreature_ResolveBuffTargetUnit
	int* get_timed_stats_subobject; // 0x018 (24) - hero 0x00A747A0 func_CombatHero_GetTimedStatsSubobject; creature 0x008A08D0 func_CombatCreature_GetTimedStatsSubobject
	int* get_owner_army_slot; // 0x01C (28) - hero 0x00BC2A10 func_CombatHero_ReturnOne; creature 0x008A42C0 func_CombatCreature_GetOwnerArmySlot
	int* get_timeline_field_7c; // 0x020 (32) - 0x00A69F00 [shared] func_CombatUnit_GetTimelineField_7C
	int* ensure_portrait_loaded; // 0x024 (36) - hero 0x00BBFE20 func_CombatHero_EnsurePortraitLoaded → IHero+0x284; creature 0x008A1160 func_CombatCreature_EnsurePortraitLoaded
	GetActiveBuffById get_active_buff; // 0x028 (40) - 0x00A6D330 [shared] func_CombatUnit_GetActiveBuff
	int* get_buff_stack_state; // 0x02C (44) - 0x00A6D380 [shared] func_CombatUnit_GetBuffStackState
	int* collect_linked_buff_ids; // 0x030 (48) - 0x00A6EEE0 [shared] func_CombatUnit_CollectLinkedBuffIds
	int* get_name_wstring; // 0x034 (52) - hero 0x00BBFE30 func_CombatHero_GetNameWString → IHero+0x268; creature 0x008A3240 func_CombatCreature_GetNameWString
	int* play_anim_simple; // 0x038 (56) - 0x00A69E20 [shared] func_CombatUnit_PlayAnimSimple
	int* get_anim_position; // 0x03C (60) - 0x00A69E60 [shared] func_CombatUnit_GetAnimPosition
	int* get_position_2d; // 0x040 (64) - 0x00A6A040 [shared] func_CombatUnit_GetPosition2D
	int* get_owner_side_kind; // 0x044 (68) - hero 0x00BBF3F0 func_CombatHero_GetOwnerSideKind (ret 2); creature 0x008A1980 func_CombatCreature_GetCreatureDbEntry
	int* get_speed_modifier; // 0x048 (72) - hero 0x00BBF5B0 func_CombatHero_ReturnZero; creature 0x008A1E50 func_CombatCreature_ComputeSpeedModifiers
	int* get_type_id; // 0x04C (76) - 0x00B096E0 [shared] func_CombatUnit_GetTypeId
	int* get_creature_grade_byte; // 0x050 (80) - hero 0x00458480 func_ReturnOne; creature 0x008A3320 func_CombatCreature_GetGradeByte
	int* is_owner_matching; // 0x054 (84) - hero 0x00BBF560 func_CombatHero_ReturnZeroB; creature 0x008A4250 func_CombatCreature_IsOwnerMatching
	int* get_state_flag_70; // 0x058 (88) - hero 0x00412B40 func_ReturnFalse; creature 0x008AE210 func_CombatCreature_GetStateFlag70
	int* get_state_flag_7c; // 0x05C (92) - hero 0x00BBF550 func_CombatHero_ReturnFalse; creature 0x008A0F20 func_CombatCreature_GetStateFlag7C
	int* get_combat_unit_ptr; // 0x060 (96) - 0x008AE230 [shared] func_CombatUnit_GetCombatUnitPtr
	int* get_combat_unit_ptr_mi; // 0x064 (100) - 0x008AE230 [shared] func_CombatUnit_GetCombatUnitPtr
	int* build_runtime_data; // 0x068 (104) - hero 0x00BBFBE0 func_CombatHero_NotifyOwnerRefresh; creature 0x008AC990 func_CombatCreature_BuildRuntimeData
	int* get_side_ptr_a; // 0x06C (108) - hero 0x00C98940 func_CombatUnit_ReturnZero; creature 0x008AE260 func_CombatCreature_GetSidePtr
	int* get_side_ptr_b; // 0x070 (112) - hero 0x00C98940 func_CombatUnit_ReturnZero; creature 0x008AE260 func_CombatCreature_GetSidePtr
	int* get_hero_aux_subobject; // 0x074 (116) - hero 0x00BC2B30 func_CombatHero_GetAuxSubobject; creature 0x00C98940 func_CombatUnit_ReturnZero
	int* get_hero_aux_subobject_b; // 0x078 (120) - hero 0x00BC2B30 func_CombatHero_GetAuxSubobject; creature 0x00C98940 func_CombatUnit_ReturnZero
	int* stub_return_zero_a; // 0x07C (124) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* stub_return_zero_b; // 0x080 (128) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* stub_return_zero_c; // 0x084 (132) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* stub_return_zero_d; // 0x088 (136) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* stub_return_zero_e; // 0x08C (140) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* stub_return_zero_f; // 0x090 (144) - 0x00C98940 [shared] func_CombatUnit_ReturnZero
	int* refresh_unit_data; // 0x094 (148) - 0x008AEDE0 [shared] func_CombatUnit_RefreshUnitData
	int* on_combat_event; // 0x098 (152) - 0x00A70650 [shared] func_CombatUnit_OnCombatEvent
	int* schedule_linked_spell_fx; // 0x09C (156) - hero 0x00BC0EB0 func_CombatHero_ScheduleLinkedSpellFx; creature 0x008AAAA0 func_CombatCreature_ScheduleLinkedSpellFx
	int* get_position_ptr; // 0x0A0 (160) - 0x008AE2C0 [shared] func_CombatUnit_GetPositionPtr
	int* get_facing; // 0x0A4 (164) - 0x008AE2E0 [shared] func_CombatUnit_GetFacing
	int* compute_attack_anim_pos; // 0x0A8 (168) - 0x00A6A590 [shared] func_CombatUnit_ComputeAttackAnimPos
	int* on_siege_anim_hook; // 0x0AC (172) - 0x00A70420 [shared] func_CombatUnit_OnSiegeAnimHook
	int* get_visual_id; // 0x0B0 (176) - 0x008AEB30 [shared] func_CombatUnit_GetVisualId
	int* get_anim_duration; // 0x0B4 (180) - 0x00A70450 [shared] func_CombatUnit_GetAnimDuration
	int* stub_noop; // 0x0B8 (184) - hero 0x00CC2180 func_CombatHero_Noop; creature 0x008AA990 func_CombatCreature_Noop
	int* begin_tile_query; // 0x0BC (188) - 0x00A6D810 [shared] func_CombatUnit_BeginTileQuery
	int* has_tile_pair; // 0x0C0 (192) - 0x00A6D980 [shared] func_CombatUnit_HasTilePair
	int* mark_current_tile; // 0x0C4 (196) - 0x00A6D870 [shared] func_CombatUnit_MarkCurrentTile
	int* mark_tiles_from_position; // 0x0C8 (200) - 0x00A6D8B0 [shared] func_CombatUnit_MarkTilesFromPosition
	int* try_move_to_tile; // 0x0CC (204) - 0x00A6DA30 [shared] func_CombatUnit_TryMoveToTile
	int* on_timeline_refresh; // 0x0D0 (208) - hero 0x00BC0910 func_CombatHero_OnTimelineRefresh; creature 0x008A6210 func_CombatCreature_OnTimelineRefresh
	int* notify_timeline_sub; // 0x0D4 (212) - hero 0x00A6B180 func_CombatHero_NotifyTimelineSub; creature 0x008A6330 func_CombatCreature_NotifyTimelineSub
	int* get_side_visual_id; // 0x0D8 (216) - hero 0x00BBF3C0 func_CombatHero_GetSideVisualId (ret 2); creature 0x008A3340 func_CombatCreature_GetVisualByte
	int* set_needs_refresh; // 0x0DC (220) - 0x00A6A910 [shared] func_CombatUnit_SetNeedsRefresh
	int* get_refresh_bool; // 0x0E0 (224) - hero 0x00412B40 func_ReturnFalse; creature 0x00412BD0 func_ReturnTrue
	int* notify_owner_vtable_e0; // 0x0E4 (228) - 0x008AE3A0 [shared] func_CombatUnit_NotifyOwnerVtableE0
	int* set_facing_from_timeline; // 0x0E8 (232) - hero 0x00A6E3C0 func_CombatHero_SetFacingFromTimeline; creature 0x008A09E0 func_CombatCreature_SetFacingFromTimeline
	int* on_siege_subhook; // 0x0EC (236) - hero 0x008B1A30 func_CombatHero_OnSiegeSubhook; creature 0x008AE3E0 func_CombatCreature_OnSiegeSubhook
	int* ret_void_noop; // 0x0F0 (240) - hero 0x00782CF0 func_CombatHero_RetVoid; creature 0x008AE420 func_CombatCreature_RetVoid
	int* is_side_zero; // 0x0F4 (244) - 0x008AE450 [shared] func_CombatUnit_IsSideZero
	int* get_anim_object; // 0x0F8 (248) - 0x008AED40 [shared] func_CombatUnit_GetAnimObject
	int* notify_timeline_change; // 0x0FC (252) - 0x00A6A2E0 [shared] func_CombatUnit_NotifyTimelineChange
	int* schedule_buff_anim; // 0x100 (256) - 0x00A6F5B0 [shared] func_CombatUnit_ScheduleBuffAnim
	int* schedule_spell_anim; // 0x104 (260) - 0x00A6F780 [shared] func_CombatUnit_ScheduleSpellAnim
	int* schedule_attack_anim; // 0x108 (264) - 0x00A6BEC0 [shared] func_CombatUnit_ScheduleAttackAnim
	int* advance_timeline; // 0x10C (268) - 0x00A6A370 [shared] func_CombatUnit_AdvanceTimeline
	int* advance_timeline_to_target; // 0x110 (272) - 0x00A6C010 [shared] func_CombatUnit_AdvanceTimelineToTarget
	int* schedule_timeline_hook_114; // 0x114 (276) - hero 0x00BC07E0 func_CombatHero_ScheduleTimelineHook114; creature 0x008A9160 func_CombatCreature_ScheduleTimelineHook114
	int* schedule_timeline_hook_118; // 0x118 (280) - hero 0x00A6DBA0 func_CombatHero_ScheduleTimelineHook118; creature 0x008A5FB0 func_CombatCreature_ScheduleTimelineHook118
	int* schedule_move_path; // 0x11C (284) - 0x00A6CBC0 [shared] func_CombatUnit_ScheduleMovePath
	int* schedule_generic_anim; // 0x120 (288) - 0x00A6FEF0 [shared] func_CombatUnit_ScheduleGenericAnim
	int* schedule_ranged_attack; // 0x124 (292) - 0x00A71810 [shared] func_CombatUnit_ScheduleRangedAttack
	int* schedule_timeline_hook_128; // 0x128 (296) - hero 0x00BC1070 func_CombatHero_ScheduleTimelineHook128; creature 0x008AE510 func_CombatCreature_ScheduleTimelineHook128
	int* schedule_timeline_hook_12c; // 0x12C (300) - hero 0x00BC08C0 func_CombatHero_ScheduleTimelineHook12C; creature 0x008A9220 func_CombatCreature_ScheduleTimelineHook12C
	int* schedule_hit_react; // 0x130 (304) - 0x00A713C0 [shared] func_CombatUnit_ScheduleHitReact
	int* schedule_death_anim; // 0x134 (308) - 0x00A6C990 [shared] func_CombatUnit_ScheduleDeathAnim
	int* schedule_timed_anim; // 0x138 (312) - 0x00A6C710 [shared] func_CombatUnit_ScheduleTimedAnim
	int* schedule_timeline_hook_13c; // 0x13C (316) - hero 0x00A6FA50 func_CombatHero_ScheduleTimelineHook13C; creature 0x008A63C0 func_CombatCreature_ScheduleTimelineHook13C
	int* play_anim_at_target; // 0x140 (320) - 0x00A6A420 [shared] func_CombatUnit_PlayAnimAtTarget
	int* schedule_multi_projectile; // 0x144 (324) - 0x00A71AD0 [shared] func_CombatUnit_ScheduleMultiProjectile
	int* schedule_phase_anim; // 0x148 (328) - 0x00A71E40 [shared] func_CombatUnit_SchedulePhaseAnim
	int* begin_action_then_notify; // 0x14C (332) - 0x00A6DDD0 [shared] func_CombatUnit_BeginActionThenNotify
	int* schedule_morale_anim; // 0x150 (336) - 0x00A72220 [shared] func_CombatUnit_ScheduleMoraleAnim
	int* on_timeline_reached; // 0x154 (340) - 0x00A71010 [shared] func_CombatUnit_OnTimelineReached
	int* schedule_timeline_hook_158; // 0x158 (344) - hero 0x00A6C140 func_CombatHero_ScheduleTimelineHook158; creature 0x008A6080 func_CombatCreature_ScheduleTimelineHook158
	int* get_timed_stats_ptr; // 0x15C (348) - 0x008AE5C0 [shared] func_CombatUnit_GetTimedStatsPtr
	int* schedule_timeline_hook_160; // 0x160 (352) - hero 0x00A6CD80 func_CombatHero_ScheduleTimelineHook160; creature 0x008A8140 func_CombatCreature_ScheduleTimelineHook160
	int* schedule_timeline_hook_164; // 0x164 (356) - hero 0x00A6CF90 func_CombatHero_ScheduleTimelineHook164; creature 0x008A0DB0 func_CombatCreature_ScheduleTimelineHook164
	int* schedule_bad_luck_anim; // 0x168 (360) - 0x00A6D140 [shared] func_CombatUnit_ScheduleBadLuckAnim
	int* schedule_good_luck_anim; // 0x16C (364) - 0x00A6D1F0 [shared] func_CombatUnit_ScheduleGoodLuckAnim
	int* get_max_timeline_frame; // 0x170 (368) - 0x00A69F10 [shared] func_CombatUnit_GetMaxTimelineFrame
	int* get_initiative_factor; // 0x174 (372) - 0x00A6A060 [shared] func_CombatUnit_GetInitiativeFactor
	int* on_control_handoff; // 0x178 (376) - 0x00A6A260 [shared] func_CombatUnit_OnControlHandoff
	int* compute_next_turn_frame; // 0x17C (380) - 0x00A6E440 [shared] func_CombatUnit_ComputeNextTurnFrame
	int* reset_atb_to_current; // 0x180 (384) - 0x00A6A210 [shared] func_CombatUnit_ResetATBToCurrent
	GetATBInfo get_atb_info_dup; // 0x184 (388) - 0x008AEB40 [shared] func_CombatUnit_GetATBInfo
	GetATBInfo get_atb_info; // 0x188 (392) - 0x008AEB40 [shared] func_CombatUnit_GetATBInfo
	SetATBValue set_atb; // 0x18C (396) - 0x00A6B690 [shared] func_SetUnitATB
	int* get_speed_penalty; // 0x190 (400) is_flying — hero 0x00412BD0 func_ReturnTrue; creature 0x008A22D0 → func_GetCreatureFlying @ 0x004BA950 (SCreature.Flying @ +0x60)
	int* get_anim_debug_name; // 0x194 (404) - 0x00A6DAE0 [shared] func_CombatUnit_GetAnimDebugName
	GetStat get_attack; // 0x198 (408) - hero 0x00BBFE50 func_GetHeroAttackCombat; creature 0x008A4800 func_GetUnitAttack
	GetStat get_defense; // 0x19C (412) - hero 0x00BBFE60 func_GetHeroDefenseCombat; creature 0x008A32B0 func_GetUnitDefense
	int* get_field_90; // 0x1A0 (416) - hero 0x00BBF5A0 func_CombatHero_ReturnZero; creature 0x008A1010 func_CombatCreature_GetField90
	int* get_combat_bonus; // 0x1A4 (420) - hero 0x00AA2E60 func_CombatHero_GetCombatBonus; creature 0x008A1590 func_CombatCreature_GetCombatBonus
	int* get_creature_tier_flag; // 0x1A8 (424) - hero 0x00BBF5C0 func_CombatHero_ReturnZero; creature 0x008A0F30 func_CombatCreature_GetTierFlag
	GetStat get_luck; // 0x1AC (428) - 0x00A6BD10 [shared] func_CombatUnit_GetLuck
	GetStat get_morale; // 0x1B0 (432) - 0x00A6BD50 [shared] func_CombatUnit_GetMorale
	GetStat get_luck_raw; // 0x1B4 (436) - hero 0x00BBFF00 func_GetCreatureLuckDebuffContext; creature 0x00A6ABF0 func_GetUnitLuck
	GetStat get_morale_raw; // 0x1B8 (440) - hero 0x00BBFFC0 func_GetCreatureMoraleDebuffContext; creature 0x00A6ACD0 func_GetUnitMorale
	GetStatFloat get_initiative; // 0x1BC (444) - hero 0x00BC1EB0 func_GetHeroInitiative; creature 0x008A4330 func_CombatCreature_GetInitiative
	int* get_shooter_flag; // 0x1C0 (448) - hero 0x00BC2A10 func_CombatHero_ReturnOne; creature 0x008A0F90 func_CombatCreature_GetShooterFlag
	int* get_ranged_penalty; // 0x1C4 (452) - hero 0x00C98940 func_CombatUnit_ReturnZero; creature 0x008A2610 func_CombatCreature_GetRangedPenalty
	int* stub_return_false; // 0x1C8 (456) - hero 0x00412B40 func_ReturnFalse; creature 0x008A3E90 func_CombatCreature_StubReturnFalse
	int* can_cast_as_creature; // 0x1CC (460) - hero 0x00412B40 func_ReturnFalse; creature 0x00A6BA00 func_CombatCreature_CanCast
	int* on_buff_refresh; // 0x1D0 (464) - hero 0x00A6B820 func_CombatHero_OnBuffRefresh; creature 0x008A3EF0 func_CombatCreature_OnBuffRefresh
	int* get_stack_constant; // 0x1D4 (468) - hero 0x008B6BD0 func_CombatHero_ReturnConstant100; creature 0x008A4270 func_CombatCreature_GetStackCount
	int* get_unit_kind_dup; // 0x1D8 (472) - hero 0x008B1E10 func_CombatCreature_GetUnitKind; creature 0x008A42E0 func_CombatCreature_GetTier
	int* get_hero_stat_stub_dc; // 0x1DC (476) - hero 0x00BBF580 func_CombatHero_GetStatStubDC; creature 0x008A4190 func_CombatCreature_GetStatStubDC
	int* get_hero_stat_stub_e0; // 0x1E0 (480) - hero 0x00BBF570 func_CombatHero_GetStatStubE0; creature 0x008A1020 func_CombatCreature_GetStatStubE0
	int* stub_return_false_e4; // 0x1E4 (484) - hero 0x00412B40 func_ReturnFalse; creature 0x008A10E0 func_CombatCreature_StubReturnFalseE4
	int* ret_void_noop_e8; // 0x1E8 (488) - hero 0x00782CF0 func_CombatHero_RetVoid; creature 0x008A1120 func_CombatCreature_RetVoidE8
	int* get_hero_stat_stub_ec; // 0x1EC (492) - hero 0x00BBF5D0 func_CombatHero_GetStatStubEC; creature 0x008A2910 func_CombatCreature_GetStatStubEC
	int* get_hero_stat_stub_f0; // 0x1F0 (496) - hero 0x00BBF5E0 func_CombatHero_GetStatStubF0; creature 0x008A2930 func_CombatCreature_GetStatStubF0
	int* get_hero_stat_stub_f4; // 0x1F4 (500) - hero 0x00BBF5F0 func_CombatHero_GetStatStubF4; creature 0x008A6E20 func_CombatCreature_GetStatStubF4
	int* stub_noop_f8; // 0x1F8 (504) - hero 0x00CC2180 func_CombatHero_Noop; creature 0x008A7780 func_CombatCreature_NoopF8
	int* get_hero_stat_stub_fc; // 0x1FC (508) - hero 0x00BBF600 func_CombatHero_GetStatStubFC; creature 0x008A40C0 func_CombatCreature_GetStatStubFC
	int* get_hero_siege_hook_200; // 0x200 (512) - hero 0x008B1D90 func_CombatHero_OnSiegeHook200; creature 0x008AA960 func_CombatCreature_OnSiegeHook200
	int* on_timeline_notify_204; // 0x204 (516) - hero 0x00A6B420 func_CombatHero_OnTimelineNotify204; creature 0x008A6BE0 func_CombatCreature_OnTimelineNotify204
	int* ret_void_noop_208; // 0x208 (520) - hero 0x00782CF0 func_CombatHero_RetVoid; creature 0x008AE750 func_CombatCreature_RetVoid208
	int* stub_return_false_20c; // 0x20C (524) - hero 0x00412B40 func_ReturnFalse; creature 0x008AE770 func_CombatCreature_StubReturnFalse20C
	int* on_wait_atb; // 0x210 (528) - hero 0x00A6B570 func_CombatWaitATB; creature 0x008A1990 func_CombatCreature_OnWaitATB
	int* ret_void_noop_214; // 0x214 (532) - hero 0x00782CF0 func_CombatHero_RetVoid; creature 0x008A0AB0 func_CombatCreature_RetVoid214
	int* ret_void_noop_218; // 0x218 (536) - hero 0x00782CF0 func_CombatHero_RetVoid; creature 0x008A4B70 func_CombatCreature_RetVoid218
	int* stub_return_zero_g; // 0x21C (540) - hero 0x00A332C0 func_CombatUnit_ReturnZero; creature 0x008A15B0 func_CombatCreature_StubReturnZero
	int* get_mana_context; // 0x220 (544) - hero 0x00BC0030 func_CombatHero_GetManaContext; creature 0x00A6BB10 func_CombatCreature_GetManaContext
	int* get_hero_stat_stub_224; // 0x224 (548) - hero 0x00BBF610 func_CombatHero_GetStatStub224; creature 0x008A11C0 func_CombatCreature_GetStatStub224
	int* get_current_mana_combat; // 0x228 (552) - hero 0x00BBFE00 func_GetHeroCurrentManaCombat; creature 0x00A739D0 func_CombatCreature_GetCurrentMana
	int* get_hero_mana_stub_22c; // 0x22C (556) - hero 0x00BBFAC0 func_CombatHero_GetManaStub22C; creature 0x008AE7E0 func_CombatCreature_GetManaStub22C
	int* get_hero_mana_stub_230; // 0x230 (560) - hero 0x00BBFAE0 func_CombatHero_GetManaStub230; creature 0x00CE6720 func_CombatCreature_GetManaStub230
	int* get_hero_mana_stub_234; // 0x234 (564) - hero 0x00BBFB80 func_GetHeroCurrentManaCombat; creature 0x008AE820 func_CombatCreature_GetManaStub234
	GetStat get_mana; // 0x238 (568) - hero 0x00BC0700 func_GetHeroStatMaxMana; creature 0x008A2230 func_CombatCreature_GetManaPoints
	GetStat get_hero_level; // 0x23C (572) - hero 0x00BBFB90 func_CombatHero_GetLevel; creature 0x008A2280 func_CombatCreature_GetLevel
	GetCombatSpellpower get_spell_mastery; // 0x240 (576) - 0x00A6A700 [shared] func_CombatUnit_GetSpellMastery
	GetCombatSpellpower get_spellpower; // 0x244 (580) - 0x00A6A7E0 [shared] func_CombatUnit_GetSpellpower
	int* get_spellbook_ptr; // 0x248 (584) - hero 0x00BC1FA0 func_CombatHero_GetSpellbookPtr; creature 0x00A73A50 func_CombatCreature_GetSpellRef
	int* get_active_spell_effects; // 0x24C (588) - 0x00A6EE70 [shared] func_GetActiveSpellEffects
	int* get_buff_list; // 0x250 (592) - 0x00A6AB80 [shared] func_CombatUnit_GetBuffList
	int* apply_active_spell_effects; // 0x254 (596) - 0x00A729B0 [shared] func_CombatUnit_ApplyActiveSpellEffects
	int* apply_soul_drain_effects; // 0x258 (600) - 0x00A72A30 [shared] func_CombatUnit_ApplySoulDrainEffects
	int* apply_spell_effect_4arg; // 0x25C (604) - 0x00A73D40 [shared] func_CombatUnit_ApplySpellEffect4
	int* apply_spell_effect_5arg; // 0x260 (608) - 0x00A73CE0 [shared] func_CombatUnit_ApplySpellEffect5
	int* apply_spell_effect_0arg; // 0x264 (612) - 0x00A73DA0 [shared] func_CombatUnit_ApplySpellEffect0
	int* remove_buff_by_id; // 0x268 (616) - 0x00A72C80 [shared] func_CombatUnit_RemoveBuffById
	int* set_buff_param; // 0x26C (620) - 0x00A6BCC0 [shared] func_CombatUnit_SetBuffParam
	int* remove_buff_filtered; // 0x270 (624) - 0x00A72D30 [shared] func_CombatUnit_RemoveBuffFiltered
	int* refresh_artifact_buffs; // 0x274 (628) - 0x00A6DF20 [shared] func_CombatUnit_RefreshArtifactBuffs
	int* play_melee_attack_anim; // 0x278 (632) - 0x00A70B20 [shared] func_CombatUnit_PlayMeleeAttackAnim
	int* play_ranged_attack_anim; // 0x27C (636) - 0x00A706E0 [shared] func_CombatUnit_PlayRangedAttackAnim
	int* play_anim_dispatch; // 0x280 (640) - 0x00A69F20 [shared] func_CombatUnit_PlayAnimDispatch
	int* play_attack_anim_at; // 0x284 (644) - 0x00A70D90 [shared] func_CombatUnit_PlayAttackAnimAt
	int* play_positional_anim; // 0x288 (648) - 0x00A69F90 [shared] func_CombatUnit_PlayPositionalAnim
	HasCreatureAbility has_ability; // 0x28C (652) - hero 0x00A332C0 func_CombatUnit_ReturnZero; creature 0x008A34C0 func_CombatCreature_HasAbility
	GetSkillMastery has_skill; // 0x290 (656) - hero 0x00BBFED0 func_HasHeroSkill; creature 0x00A332C0 func_CombatUnit_ReturnZero
	HasHeroSpec has_hero_spec; // 0x294 (660) - hero 0x00BBFEF0 func_HasHeroSpec; creature 0x00A332C0 func_CombatUnit_ReturnZero
	int* add_spell_link; // 0x298 (664) - 0x00A73DE0 [shared] func_CombatUnit_AddSpellLink
	int* remove_spell_link_pair; // 0x29C (668) - 0x00A72E00 [shared] func_CombatUnit_RemoveSpellLinkPair
	int* remove_spell_links_by_subid; // 0x2A0 (672) - 0x00A72F00 [shared] func_CombatUnit_RemoveSpellLinksBySubId
	int* remove_spell_links_by_id; // 0x2A4 (676) - 0x00A72E90 [shared] func_CombatUnit_RemoveSpellLinksById
	int* has_spell_link; // 0x2A8 (680) - 0x00A6D440 [shared] func_CombatUnit_HasSpellLink
};

using CombatHero_vtable = CombatUnit_vtable;
using CombatCreature_vtable = CombatUnit_vtable;

struct ICombatUnit {
	CombatUnit_vtable* instance;

	int* unit_ref() {
		return instance->get_unit_ref((int*)this);
	}
	int* atb_info() {
		return instance->get_atb_info((int*)this);
	}
	float atb() {
		return *(float*)(atb_info() + 0x1C / 4);
	}
	void set_atb(float value) {
		instance->set_atb((int*)this, value);
	}
	int attack() {
		return instance->get_attack((int*)this);
	}
	int defense() {
		return instance->get_defense((int*)this);
	}
	int luck() {
		return instance->get_luck((int*)this);
	}
	int morale() {
		return instance->get_morale((int*)this);
	}
	float initiative() {
		return instance->get_initiative((int*)this);
	}
	int active_buff(int ability_id) {
		return instance->get_active_buff((int*)this, ability_id);
	}
	bool is_flying() {
		typedef char(__thiscall* IsFlyingFn)(int*);
		return ((IsFlyingFn)instance->get_speed_penalty)((int*)this) != 0;
	}
};

#endif
