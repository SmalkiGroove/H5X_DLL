#!/usr/bin/env python3
"""One-shot generator for CombatUnit_vtable field names (shared vs type-specific)."""

from __future__ import annotations

from pathlib import Path

SHARED: dict[int, tuple[str, str, str]] = {
    0x004: ("scalar_deleting_dtor", "0x008AEDC0", "func_CombatUnit_ScalarDeletingDtor"),
    0x008: ("get_owner_ref", "0x008ACDC0", "func_CombatUnit_GetOwnerRef"),
    0x00C: ("get_unit_ref", "0x008AEAF0", "func_CombatUnit_GetUnitRef"),
    0x010: ("notify_owner", "0x00A6B0E0", "func_CombatUnit_NotifyOwner"),
    0x020: ("get_timeline_field_7c", "0x00A69F00", "func_CombatUnit_GetTimelineField_7C"),
    0x028: ("get_active_buff", "0x00A6D330", "func_CombatUnit_GetActiveBuff"),
    0x02C: ("get_buff_stack_state", "0x00A6D380", "func_CombatUnit_GetBuffStackState"),
    0x030: ("collect_linked_buff_ids", "0x00A6EEE0", "func_CombatUnit_CollectLinkedBuffIds"),
    0x038: ("play_anim_simple", "0x00A69E20", "func_CombatUnit_PlayAnimSimple"),
    0x03C: ("get_anim_position", "0x00A69E60", "func_CombatUnit_GetAnimPosition"),
    0x040: ("get_position_2d", "0x00A6A040", "func_CombatUnit_GetPosition2D"),
    0x04C: ("get_type_id", "0x00B096E0", "func_CombatUnit_GetTypeId"),
    0x060: ("get_combat_unit_ptr", "0x008AE230", "func_CombatUnit_GetCombatUnitPtr"),
    0x064: ("get_combat_unit_ptr_mi", "0x008AE230", "func_CombatUnit_GetCombatUnitPtr"),
    0x07C: ("stub_return_zero_a", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x080: ("stub_return_zero_b", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x084: ("stub_return_zero_c", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x088: ("stub_return_zero_d", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x08C: ("stub_return_zero_e", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x090: ("stub_return_zero_f", "0x00C98940", "func_CombatUnit_ReturnZero"),
    0x094: ("refresh_unit_data", "0x008AEDE0", "func_CombatUnit_RefreshUnitData"),
    0x098: ("on_combat_event", "0x00A70650", "func_CombatUnit_OnCombatEvent"),
    0x0A0: ("get_position_ptr", "0x008AE2C0", "func_CombatUnit_GetPositionPtr"),
    0x0A4: ("get_facing", "0x008AE2E0", "func_CombatUnit_GetFacing"),
    0x0A8: ("compute_attack_anim_pos", "0x00A6A590", "func_CombatUnit_ComputeAttackAnimPos"),
    0x0AC: ("on_siege_anim_hook", "0x00A70420", "func_CombatUnit_OnSiegeAnimHook"),
    0x0B0: ("get_visual_id", "0x008AEB30", "func_CombatUnit_GetVisualId"),
    0x0B4: ("get_anim_duration", "0x00A70450", "func_CombatUnit_GetAnimDuration"),
    0x0BC: ("begin_tile_query", "0x00A6D810", "func_CombatUnit_BeginTileQuery"),
    0x0C0: ("has_tile_pair", "0x00A6D980", "func_CombatUnit_HasTilePair"),
    0x0C4: ("mark_current_tile", "0x00A6D870", "func_CombatUnit_MarkCurrentTile"),
    0x0C8: ("mark_tiles_from_position", "0x00A6D8B0", "func_CombatUnit_MarkTilesFromPosition"),
    0x0CC: ("try_move_to_tile", "0x00A6DA30", "func_CombatUnit_TryMoveToTile"),
    0x0DC: ("set_needs_refresh", "0x00A6A910", "func_CombatUnit_SetNeedsRefresh"),
    0x0E4: ("notify_owner_vtable_e0", "0x008AE3A0", "func_CombatUnit_NotifyOwnerVtableE0"),
    0x0F4: ("is_side_zero", "0x008AE450", "func_CombatUnit_IsSideZero"),
    0x0F8: ("get_anim_object", "0x008AED40", "func_CombatUnit_GetAnimObject"),
    0x0FC: ("notify_timeline_change", "0x00A6A2E0", "func_CombatUnit_NotifyTimelineChange"),
    0x100: ("schedule_buff_anim", "0x00A6F5B0", "func_CombatUnit_ScheduleBuffAnim"),
    0x104: ("schedule_spell_anim", "0x00A6F780", "func_CombatUnit_ScheduleSpellAnim"),
    0x108: ("schedule_attack_anim", "0x00A6BEC0", "func_CombatUnit_ScheduleAttackAnim"),
    0x10C: ("advance_timeline", "0x00A6A370", "func_CombatUnit_AdvanceTimeline"),
    0x110: ("advance_timeline_to_target", "0x00A6C010", "func_CombatUnit_AdvanceTimelineToTarget"),
    0x11C: ("schedule_move_path", "0x00A6CBC0", "func_CombatUnit_ScheduleMovePath"),
    0x120: ("schedule_generic_anim", "0x00A6FEF0", "func_CombatUnit_ScheduleGenericAnim"),
    0x124: ("schedule_ranged_attack", "0x00A71810", "func_CombatUnit_ScheduleRangedAttack"),
    0x130: ("schedule_hit_react", "0x00A713C0", "func_CombatUnit_ScheduleHitReact"),
    0x134: ("schedule_death_anim", "0x00A6C990", "func_CombatUnit_ScheduleDeathAnim"),
    0x138: ("schedule_timed_anim", "0x00A6C710", "func_CombatUnit_ScheduleTimedAnim"),
    0x140: ("play_anim_at_target", "0x00A6A420", "func_CombatUnit_PlayAnimAtTarget"),
    0x144: ("schedule_multi_projectile", "0x00A71AD0", "func_CombatUnit_ScheduleMultiProjectile"),
    0x148: ("schedule_phase_anim", "0x00A71E40", "func_CombatUnit_SchedulePhaseAnim"),
    0x14C: ("begin_action_then_notify", "0x00A6DDD0", "func_CombatUnit_BeginActionThenNotify"),
    0x150: ("schedule_morale_anim", "0x00A72220", "func_CombatUnit_ScheduleMoraleAnim"),
    0x154: ("on_timeline_reached", "0x00A71010", "func_CombatUnit_OnTimelineReached"),
    0x15C: ("get_timed_stats_ptr", "0x008AE5C0", "func_CombatUnit_GetTimedStatsPtr"),
    0x168: ("schedule_bad_luck_anim", "0x00A6D140", "func_CombatUnit_ScheduleBadLuckAnim"),
    0x16C: ("schedule_good_luck_anim", "0x00A6D1F0", "func_CombatUnit_ScheduleGoodLuckAnim"),
    0x170: ("get_max_timeline_frame", "0x00A69F10", "func_CombatUnit_GetMaxTimelineFrame"),
    0x174: ("get_initiative_factor", "0x00A6A060", "func_CombatUnit_GetInitiativeFactor"),
    0x178: ("on_control_handoff", "0x00A6A260", "func_CombatUnit_OnControlHandoff"),
    0x17C: ("compute_next_turn_frame", "0x00A6E440", "func_CombatUnit_ComputeNextTurnFrame"),
    0x180: ("reset_atb_to_current", "0x00A6A210", "func_CombatUnit_ResetATBToCurrent"),
    0x184: ("get_atb_info_dup", "0x008AEB40", "func_CombatUnit_GetATBInfo"),
    0x188: ("get_atb_info", "0x008AEB40", "func_CombatUnit_GetATBInfo"),
    0x18C: ("set_atb", "0x00A6B690", "func_SetUnitATB"),
    0x194: ("get_anim_debug_name", "0x00A6DAE0", "func_CombatUnit_GetAnimDebugName"),
    0x1AC: ("get_luck", "0x00A6BD10", "func_CombatUnit_GetLuck"),
    0x1B0: ("get_morale", "0x00A6BD50", "func_CombatUnit_GetMorale"),
    0x240: ("get_spell_mastery", "0x00A6A700", "func_CombatUnit_GetSpellMastery"),
    0x244: ("get_spellpower", "0x00A6A7E0", "func_CombatUnit_GetSpellpower"),
    0x24C: ("get_active_spell_effects", "0x00A6EE70", "func_GetActiveSpellEffects"),
    0x250: ("get_buff_list", "0x00A6AB80", "func_CombatUnit_GetBuffList"),
    0x254: ("apply_active_spell_effects", "0x00A729B0", "func_CombatUnit_ApplyActiveSpellEffects"),
    0x258: ("apply_soul_drain_effects", "0x00A72A30", "func_CombatUnit_ApplySoulDrainEffects"),
    0x25C: ("apply_spell_effect_4arg", "0x00A73D40", "func_CombatUnit_ApplySpellEffect4"),
    0x260: ("apply_spell_effect_5arg", "0x00A73CE0", "func_CombatUnit_ApplySpellEffect5"),
    0x264: ("apply_spell_effect_0arg", "0x00A73DA0", "func_CombatUnit_ApplySpellEffect0"),
    0x268: ("remove_buff_by_id", "0x00A72C80", "func_CombatUnit_RemoveBuffById"),
    0x26C: ("set_buff_param", "0x00A6BCC0", "func_CombatUnit_SetBuffParam"),
    0x270: ("remove_buff_filtered", "0x00A72D30", "func_CombatUnit_RemoveBuffFiltered"),
    0x274: ("refresh_artifact_buffs", "0x00A6DF20", "func_CombatUnit_RefreshArtifactBuffs"),
    0x278: ("play_melee_attack_anim", "0x00A70B20", "func_CombatUnit_PlayMeleeAttackAnim"),
    0x27C: ("play_ranged_attack_anim", "0x00A706E0", "func_CombatUnit_PlayRangedAttackAnim"),
    0x280: ("play_anim_dispatch", "0x00A69F20", "func_CombatUnit_PlayAnimDispatch"),
    0x284: ("play_attack_anim_at", "0x00A70D90", "func_CombatUnit_PlayAttackAnimAt"),
    0x288: ("play_positional_anim", "0x00A69F90", "func_CombatUnit_PlayPositionalAnim"),
    0x298: ("add_spell_link", "0x00A73DE0", "func_CombatUnit_AddSpellLink"),
    0x29C: ("remove_spell_link_pair", "0x00A72E00", "func_CombatUnit_RemoveSpellLinkPair"),
    0x2A0: ("remove_spell_links_by_subid", "0x00A72F00", "func_CombatUnit_RemoveSpellLinksBySubId"),
    0x2A4: ("remove_spell_links_by_id", "0x00A72E90", "func_CombatUnit_RemoveSpellLinksById"),
    0x2A8: ("has_spell_link", "0x00A6D440", "func_CombatUnit_HasSpellLink"),
}

TYPE_FIELDS: dict[int, tuple[str, str]] = {
    0x000: (
        "get_unit_kind",
        "hero 0x00A83AA0 func_CombatHero_GetUnitKind (ret 2); "
        "creature 0x008B1E10 func_CombatCreature_GetUnitKind (ret 1)",
    ),
    0x014: (
        "init_owner_timed_stats",
        "hero 0x008B1780 func_CombatHero_InitOwnerTimedStats; "
        "creature 0x008A0950 func_CombatCreature_ResolveBuffTargetUnit",
    ),
    0x018: (
        "get_timed_stats_subobject",
        "hero 0x00A747A0 func_CombatHero_GetTimedStatsSubobject; "
        "creature 0x008A08D0 func_CombatCreature_GetTimedStatsSubobject",
    ),
    0x01C: (
        "get_owner_army_slot",
        "hero 0x00BC2A10 func_CombatHero_ReturnOne; "
        "creature 0x008A42C0 func_CombatCreature_GetOwnerArmySlot",
    ),
    0x024: (
        "ensure_portrait_loaded",
        "hero 0x00BBFE20 func_CombatHero_EnsurePortraitLoaded → IHero+0x284; "
        "creature 0x008A1160 func_CombatCreature_EnsurePortraitLoaded",
    ),
    0x034: (
        "get_name_wstring",
        "hero 0x00BBFE30 func_CombatHero_GetNameWString → IHero+0x268; "
        "creature 0x008A3240 func_CombatCreature_GetNameWString",
    ),
    0x044: (
        "get_owner_side_kind",
        "hero 0x00BBF3F0 func_CombatHero_GetOwnerSideKind (ret 2); "
        "creature 0x008A1980 func_CombatCreature_GetCreatureDbEntry",
    ),
    0x048: (
        "get_speed_modifier",
        "hero 0x00BBF5B0 func_CombatHero_ReturnZero; "
        "creature 0x008A1E50 func_CombatCreature_ComputeSpeedModifiers",
    ),
    0x050: (
        "get_creature_grade_byte",
        "hero 0x00458480 func_ReturnOne; "
        "creature 0x008A3320 func_CombatCreature_GetGradeByte",
    ),
    0x054: (
        "is_owner_matching",
        "hero 0x00BBF560 func_CombatHero_ReturnZeroB; "
        "creature 0x008A4250 func_CombatCreature_IsOwnerMatching",
    ),
    0x058: (
        "get_state_flag_70",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x008AE210 func_CombatCreature_GetStateFlag70",
    ),
    0x05C: (
        "get_state_flag_7c",
        "hero 0x00BBF550 func_CombatHero_ReturnFalse; "
        "creature 0x008A0F20 func_CombatCreature_GetStateFlag7C",
    ),
    0x068: (
        "build_runtime_data",
        "hero 0x00BBFBE0 func_CombatHero_NotifyOwnerRefresh; "
        "creature 0x008AC990 func_CombatCreature_BuildRuntimeData",
    ),
    0x06C: (
        "get_side_ptr_a",
        "hero 0x00C98940 func_CombatUnit_ReturnZero; "
        "creature 0x008AE260 func_CombatCreature_GetSidePtr",
    ),
    0x070: (
        "get_side_ptr_b",
        "hero 0x00C98940 func_CombatUnit_ReturnZero; "
        "creature 0x008AE260 func_CombatCreature_GetSidePtr",
    ),
    0x074: (
        "get_hero_aux_subobject",
        "hero 0x00BC2B30 func_CombatHero_GetAuxSubobject; "
        "creature 0x00C98940 func_CombatUnit_ReturnZero",
    ),
    0x078: (
        "get_hero_aux_subobject_b",
        "hero 0x00BC2B30 func_CombatHero_GetAuxSubobject; "
        "creature 0x00C98940 func_CombatUnit_ReturnZero",
    ),
    0x09C: (
        "schedule_linked_spell_fx",
        "hero 0x00BC0EB0 func_CombatHero_ScheduleLinkedSpellFx; "
        "creature 0x008AAAA0 func_CombatCreature_ScheduleLinkedSpellFx",
    ),
    0x0B8: (
        "stub_noop",
        "hero 0x00CC2180 func_CombatHero_Noop; "
        "creature 0x008AA990 func_CombatCreature_Noop",
    ),
    0x0D0: (
        "on_timeline_refresh",
        "hero 0x00BC0910 func_CombatHero_OnTimelineRefresh; "
        "creature 0x008A6210 func_CombatCreature_OnTimelineRefresh",
    ),
    0x0D4: (
        "notify_timeline_sub",
        "hero 0x00A6B180 func_CombatHero_NotifyTimelineSub; "
        "creature 0x008A6330 func_CombatCreature_NotifyTimelineSub",
    ),
    0x0D8: (
        "get_side_visual_id",
        "hero 0x00BBF3C0 func_CombatHero_GetSideVisualId (ret 2); "
        "creature 0x008A3340 func_CombatCreature_GetVisualByte",
    ),
    0x0E0: (
        "get_refresh_bool",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x00412BD0 func_ReturnTrue",
    ),
    0x0E8: (
        "set_facing_from_timeline",
        "hero 0x00A6E3C0 func_CombatHero_SetFacingFromTimeline; "
        "creature 0x008A09E0 func_CombatCreature_SetFacingFromTimeline",
    ),
    0x0EC: (
        "on_siege_subhook",
        "hero 0x008B1A30 func_CombatHero_OnSiegeSubhook; "
        "creature 0x008AE3E0 func_CombatCreature_OnSiegeSubhook",
    ),
    0x0F0: (
        "ret_void_noop",
        "hero 0x00782CF0 func_CombatHero_RetVoid; "
        "creature 0x008AE420 func_CombatCreature_RetVoid",
    ),
    0x114: (
        "schedule_timeline_hook_114",
        "hero 0x00BC07E0 func_CombatHero_ScheduleTimelineHook114; "
        "creature 0x008A9160 func_CombatCreature_ScheduleTimelineHook114",
    ),
    0x118: (
        "schedule_timeline_hook_118",
        "hero 0x00A6DBA0 func_CombatHero_ScheduleTimelineHook118; "
        "creature 0x008A5FB0 func_CombatCreature_ScheduleTimelineHook118",
    ),
    0x128: (
        "schedule_timeline_hook_128",
        "hero 0x00BC1070 func_CombatHero_ScheduleTimelineHook128; "
        "creature 0x008AE510 func_CombatCreature_ScheduleTimelineHook128",
    ),
    0x12C: (
        "schedule_timeline_hook_12c",
        "hero 0x00BC08C0 func_CombatHero_ScheduleTimelineHook12C; "
        "creature 0x008A9220 func_CombatCreature_ScheduleTimelineHook12C",
    ),
    0x13C: (
        "schedule_timeline_hook_13c",
        "hero 0x00A6FA50 func_CombatHero_ScheduleTimelineHook13C; "
        "creature 0x008A63C0 func_CombatCreature_ScheduleTimelineHook13C",
    ),
    0x158: (
        "schedule_timeline_hook_158",
        "hero 0x00A6C140 func_CombatHero_ScheduleTimelineHook158; "
        "creature 0x008A6080 func_CombatCreature_ScheduleTimelineHook158",
    ),
    0x160: (
        "schedule_timeline_hook_160",
        "hero 0x00A6CD80 func_CombatHero_ScheduleTimelineHook160; "
        "creature 0x008A8140 func_CombatCreature_ScheduleTimelineHook160",
    ),
    0x164: (
        "schedule_timeline_hook_164",
        "hero 0x00A6CF90 func_CombatHero_ScheduleTimelineHook164; "
        "creature 0x008A0DB0 func_CombatCreature_ScheduleTimelineHook164",
    ),
    0x190: (
        "get_speed_penalty",
        "hero 0x00412BD0 func_ReturnTrue; "
        "creature 0x008A22D0 func_CombatCreature_GetSpeedPenalty",
    ),
    0x198: (
        "get_attack",
        "hero 0x00BBFE50 func_GetHeroAttackCombat; "
        "creature 0x008A4800 func_GetUnitAttack",
    ),
    0x19C: (
        "get_defense",
        "hero 0x00BBFE60 func_GetHeroDefenseCombat; "
        "creature 0x008A32B0 func_GetUnitDefense",
    ),
    0x1A0: (
        "get_field_90",
        "hero 0x00BBF5A0 func_CombatHero_ReturnZero; "
        "creature 0x008A1010 func_CombatCreature_GetField90",
    ),
    0x1A4: (
        "get_combat_bonus",
        "hero 0x00AA2E60 func_CombatHero_GetCombatBonus; "
        "creature 0x008A1590 func_CombatCreature_GetCombatBonus",
    ),
    0x1A8: (
        "get_creature_tier_flag",
        "hero 0x00BBF5C0 func_CombatHero_ReturnZero; "
        "creature 0x008A0F30 func_CombatCreature_GetTierFlag",
    ),
    0x1B4: (
        "get_luck_raw",
        "hero 0x00BBFF00 func_GetCreatureLuckDebuffContext; "
        "creature 0x00A6ABF0 func_GetUnitLuck",
    ),
    0x1B8: (
        "get_morale_raw",
        "hero 0x00BBFFC0 func_GetCreatureMoraleDebuffContext; "
        "creature 0x00A6ACD0 func_GetUnitMorale",
    ),
    0x1BC: (
        "get_initiative",
        "hero 0x00BC1EB0 func_GetHeroInitiative; "
        "creature 0x008A4330 func_CombatCreature_GetInitiative",
    ),
    0x1C0: (
        "get_shooter_flag",
        "hero 0x00BC2A10 func_CombatHero_ReturnOne; "
        "creature 0x008A0F90 func_CombatCreature_GetShooterFlag",
    ),
    0x1C4: (
        "get_ranged_penalty",
        "hero 0x00C98940 func_CombatUnit_ReturnZero; "
        "creature 0x008A2610 func_CombatCreature_GetRangedPenalty",
    ),
    0x1C8: (
        "stub_return_false",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x008A3E90 func_CombatCreature_StubReturnFalse",
    ),
    0x1CC: (
        "can_cast_as_creature",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x00A6BA00 func_CombatCreature_CanCast",
    ),
    0x1D0: (
        "on_buff_refresh",
        "hero 0x00A6B820 func_CombatHero_OnBuffRefresh; "
        "creature 0x008A3EF0 func_CombatCreature_OnBuffRefresh",
    ),
    0x1D4: (
        "get_stack_constant",
        "hero 0x008B6BD0 func_CombatHero_ReturnConstant100; "
        "creature 0x008A4270 func_CombatCreature_GetStackCount",
    ),
    0x1D8: (
        "get_unit_kind_dup",
        "hero 0x008B1E10 func_CombatCreature_GetUnitKind; "
        "creature 0x008A42E0 func_CombatCreature_GetTier",
    ),
    0x1DC: (
        "get_hero_stat_stub_dc",
        "hero 0x00BBF580 func_CombatHero_GetStatStubDC; "
        "creature 0x008A4190 func_CombatCreature_GetStatStubDC",
    ),
    0x1E0: (
        "get_hero_stat_stub_e0",
        "hero 0x00BBF570 func_CombatHero_GetStatStubE0; "
        "creature 0x008A1020 func_CombatCreature_GetStatStubE0",
    ),
    0x1E4: (
        "stub_return_false_e4",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x008A10E0 func_CombatCreature_StubReturnFalseE4",
    ),
    0x1E8: (
        "ret_void_noop_e8",
        "hero 0x00782CF0 func_CombatHero_RetVoid; "
        "creature 0x008A1120 func_CombatCreature_RetVoidE8",
    ),
    0x1EC: (
        "get_hero_stat_stub_ec",
        "hero 0x00BBF5D0 func_CombatHero_GetStatStubEC; "
        "creature 0x008A2910 func_CombatCreature_GetStatStubEC",
    ),
    0x1F0: (
        "get_hero_stat_stub_f0",
        "hero 0x00BBF5E0 func_CombatHero_GetStatStubF0; "
        "creature 0x008A2930 func_CombatCreature_GetStatStubF0",
    ),
    0x1F4: (
        "get_hero_stat_stub_f4",
        "hero 0x00BBF5F0 func_CombatHero_GetStatStubF4; "
        "creature 0x008A6E20 func_CombatCreature_GetStatStubF4",
    ),
    0x1F8: (
        "stub_noop_f8",
        "hero 0x00CC2180 func_CombatHero_Noop; "
        "creature 0x008A7780 func_CombatCreature_NoopF8",
    ),
    0x1FC: (
        "get_hero_stat_stub_fc",
        "hero 0x00BBF600 func_CombatHero_GetStatStubFC; "
        "creature 0x008A40C0 func_CombatCreature_GetStatStubFC",
    ),
    0x200: (
        "get_hero_siege_hook_200",
        "hero 0x008B1D90 func_CombatHero_OnSiegeHook200; "
        "creature 0x008AA960 func_CombatCreature_OnSiegeHook200",
    ),
    0x204: (
        "on_timeline_notify_204",
        "hero 0x00A6B420 func_CombatHero_OnTimelineNotify204; "
        "creature 0x008A6BE0 func_CombatCreature_OnTimelineNotify204",
    ),
    0x208: (
        "ret_void_noop_208",
        "hero 0x00782CF0 func_CombatHero_RetVoid; "
        "creature 0x008AE750 func_CombatCreature_RetVoid208",
    ),
    0x20C: (
        "stub_return_false_20c",
        "hero 0x00412B40 func_ReturnFalse; "
        "creature 0x008AE770 func_CombatCreature_StubReturnFalse20C",
    ),
    0x210: (
        "on_wait_atb",
        "hero 0x00A6B570 func_CombatWaitATB; "
        "creature 0x008A1990 func_CombatCreature_OnWaitATB",
    ),
    0x214: (
        "ret_void_noop_214",
        "hero 0x00782CF0 func_CombatHero_RetVoid; "
        "creature 0x008A0AB0 func_CombatCreature_RetVoid214",
    ),
    0x218: (
        "ret_void_noop_218",
        "hero 0x00782CF0 func_CombatHero_RetVoid; "
        "creature 0x008A4B70 func_CombatCreature_RetVoid218",
    ),
    0x21C: (
        "stub_return_zero_g",
        "hero 0x00A332C0 func_CombatUnit_ReturnZero; "
        "creature 0x008A15B0 func_CombatCreature_StubReturnZero",
    ),
    0x220: (
        "get_mana_context",
        "hero 0x00BC0030 func_CombatHero_GetManaContext; "
        "creature 0x00A6BB10 func_CombatCreature_GetManaContext",
    ),
    0x224: (
        "get_hero_stat_stub_224",
        "hero 0x00BBF610 func_CombatHero_GetStatStub224; "
        "creature 0x008A11C0 func_CombatCreature_GetStatStub224",
    ),
    0x228: (
        "get_current_mana_combat",
        "hero 0x00BBFE00 func_GetHeroCurrentManaCombat; "
        "creature 0x00A739D0 func_CombatCreature_GetCurrentMana",
    ),
    0x22C: (
        "get_hero_mana_stub_22c",
        "hero 0x00BBFAC0 func_CombatHero_GetManaStub22C; "
        "creature 0x008AE7E0 func_CombatCreature_GetManaStub22C",
    ),
    0x230: (
        "get_hero_mana_stub_230",
        "hero 0x00BBFAE0 func_CombatHero_GetManaStub230; "
        "creature 0x00CE6720 func_CombatCreature_GetManaStub230",
    ),
    0x234: (
        "get_hero_mana_stub_234",
        "hero 0x00BBFB80 func_GetHeroCurrentManaCombat; "
        "creature 0x008AE820 func_CombatCreature_GetManaStub234",
    ),
    0x238: (
        "get_mana",
        "hero 0x00BC0700 func_GetHeroStatMaxMana; "
        "creature 0x008A2230 func_CombatCreature_GetManaPoints",
    ),
    0x23C: (
        "get_hero_level",
        "hero 0x00BBFB90 func_CombatHero_GetLevel; "
        "creature 0x008A2280 func_CombatCreature_GetLevel",
    ),
    0x248: (
        "get_spellbook_ptr",
        "hero 0x00BC1FA0 func_CombatHero_GetSpellbookPtr; "
        "creature 0x00A73A50 func_CombatCreature_GetSpellRef",
    ),
    0x28C: (
        "has_ability",
        "hero 0x00A332C0 func_CombatUnit_ReturnZero; "
        "creature 0x008A34C0 func_CombatCreature_HasAbility",
    ),
    0x290: (
        "has_skill",
        "hero 0x00BBFED0 func_HasHeroSkill; "
        "creature 0x00A332C0 func_CombatUnit_ReturnZero",
    ),
    0x294: (
        "has_hero_spec",
        "hero 0x00BBFEF0 func_HasHeroSpec; "
        "creature 0x00A332C0 func_CombatUnit_ReturnZero",
    ),
}

TYPED: dict[str, str] = {
    "get_unit_ref": "GetUnitRef",
    "get_active_buff": "GetActiveBuffById",
    "get_atb_info": "GetATBInfo",
    "get_atb_info_dup": "GetATBInfo",
    "set_atb": "SetATBValue",
    "get_attack": "GetStat",
    "get_defense": "GetStat",
    "get_luck": "GetStat",
    "get_morale": "GetStat",
    "get_luck_raw": "GetStat",
    "get_morale_raw": "GetStat",
    "get_initiative": "GetStatFloat",
    "get_mana": "GetStat",
    "get_hero_level": "GetStat",
    "get_spell_mastery": "GetCombatSpellpower",
    "get_spellpower": "GetCombatSpellpower",
    "has_ability": "HasCreatureAbility",
    "has_skill": "GetSkillMastery",
    "has_hero_spec": "HasHeroSpec",
}


def field_type(name: str) -> str:
    return TYPED.get(name, "int*")


def emit_struct_lines() -> list[str]:
    lines: list[str] = []
    for off in range(0, 0x2AC, 4):
        dec = off
        idx = off
        if off in SHARED:
            name, impl, ghidra = SHARED[off]
            ftype = field_type(name)
            lines.append(
                f"\t{ftype} {name}; // 0x{off:03X} ({dec}) - {impl} [shared] {ghidra}"
            )
        elif off in TYPE_FIELDS:
            name, note = TYPE_FIELDS[off]
            ftype = field_type(name)
            lines.append(f"\t{ftype} {name}; // 0x{off:03X} ({dec}) - {note}")
        else:
            lines.append(f"\tint* call_{idx}; // 0x{off:03X} ({dec})")
    return lines


HEADER = '''#pragma once
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
\tCombatHero_vtable_addr = 0x00E8499C,
\tCombatCreature_vtable_addr = 0x00E4FBA4,
\tCombatHero_vtordisp = 0x118,
\tCombatCreature_vtordisp = 0x144
};

struct CombatUnit_vtable {
'''

FOOTER = '''
};

using CombatHero_vtable = CombatUnit_vtable;
using CombatCreature_vtable = CombatUnit_vtable;

struct ICombatUnit {
\tCombatUnit_vtable* instance;

\tint* unit_ref() {
\t\treturn instance->get_unit_ref((int*)this);
\t}
\tint* atb_info() {
\t\treturn instance->get_atb_info((int*)this);
\t}
\tfloat atb() {
\t\treturn *(float*)(atb_info() + 0x1C / 4);
\t}
\tvoid set_atb(float value) {
\t\tinstance->set_atb((int*)this, value);
\t}
\tint attack() {
\t\treturn instance->get_attack((int*)this);
\t}
\tint defense() {
\t\treturn instance->get_defense((int*)this);
\t}
\tint luck() {
\t\treturn instance->get_luck((int*)this);
\t}
\tint morale() {
\t\treturn instance->get_morale((int*)this);
\t}
\tfloat initiative() {
\t\treturn instance->get_initiative((int*)this);
\t}
\tint active_buff(int ability_id) {
\t\treturn instance->get_active_buff((int*)this, ability_id);
\t}
};

#endif
'''


def main() -> None:
    out = Path(__file__).resolve().parents[2] / "src/structs/CombatUnit.h"
    body = HEADER + "\n".join(emit_struct_lines()) + FOOTER
    out.write_text(body, encoding="utf-8", newline="\n")
    print(f"Wrote {out}")


if __name__ == "__main__":
    main()
