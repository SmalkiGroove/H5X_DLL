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
    0x000: ("call_0", "hero/creature dtor thunks"),
    0x014: ("call_16", "type-specific"),
    0x018: ("call_20", "type-specific"),
    0x01C: ("call_24", "type-specific"),
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
    0x044: ("call_44", "type-specific"),
    0x048: ("call_48", "type-specific"),
    0x050: ("call_52", "type-specific"),
    0x054: ("call_56", "type-specific"),
    0x058: ("call_60", "type-specific"),
    0x05C: ("call_64", "type-specific"),
    0x068: ("call_72", "type-specific"),
    0x06C: ("call_76", "type-specific"),
    0x070: ("call_80", "type-specific"),
    0x074: ("call_84", "type-specific"),
    0x078: ("call_88", "type-specific"),
    0x09C: ("call_100", "type-specific"),
    0x0B8: ("call_120", "type-specific"),
    0x0D0: ("call_208", "type-specific"),
    0x0D4: ("call_212", "type-specific"),
    0x0D8: ("call_216", "type-specific"),
    0x0E0: ("call_224", "type-specific"),
    0x0E8: ("call_232", "type-specific"),
    0x0EC: ("call_236", "type-specific"),
    0x0F0: ("call_240", "type-specific"),
    0x114: ("call_276", "type-specific"),
    0x118: ("call_280", "type-specific"),
    0x128: ("call_296", "type-specific"),
    0x12C: ("call_300", "type-specific"),
    0x13C: ("call_316", "type-specific"),
    0x158: ("call_344", "type-specific"),
    0x160: ("call_352", "type-specific"),
    0x164: ("call_356", "type-specific"),
    0x190: ("call_400", "type-specific"),
    0x198: ("get_attack", "hero 0x00BBFE50; creature 0x008A4800"),
    0x19C: ("get_defense", "hero 0x00BBFE60; creature 0x008A32B0"),
    0x1A0: ("call_416", "type-specific"),
    0x1A4: ("call_420", "type-specific"),
    0x1A8: ("call_424", "type-specific"),
    0x1B4: ("get_luck_raw", "hero 0x00BBFF00; creature 0x00A6ABF0"),
    0x1B8: ("get_morale_raw", "hero 0x00BBFFC0; creature 0x00A6ACD0"),
    0x1BC: ("get_initiative", "hero 0x00BC1EB0; creature 0x008A4330"),
    0x1C0: ("call_448", "type-specific"),
    0x1C4: ("call_452", "type-specific"),
    0x1C8: ("call_456", "type-specific"),
    0x1CC: ("call_460", "type-specific"),
    0x1D0: ("call_464", "type-specific"),
    0x1D4: ("call_468", "type-specific"),
    0x1D8: ("call_472", "type-specific"),
    0x1DC: ("call_476", "type-specific"),
    0x1E0: ("call_480", "type-specific"),
    0x1E4: ("call_484", "type-specific"),
    0x1E8: ("call_488", "type-specific"),
    0x1EC: ("call_492", "type-specific"),
    0x1F0: ("call_496", "type-specific"),
    0x1F4: ("call_500", "type-specific"),
    0x1F8: ("call_504", "type-specific"),
    0x1FC: ("call_508", "type-specific"),
    0x200: ("call_512", "type-specific"),
    0x204: ("call_516", "type-specific"),
    0x208: ("call_520", "type-specific"),
    0x20C: ("call_524", "type-specific"),
    0x210: ("call_528", "type-specific"),
    0x214: ("call_532", "type-specific"),
    0x218: ("call_536", "type-specific"),
    0x21C: ("stub_return_zero_g", "hero 0x00A332C0; creature 0x008A15B0"),
    0x220: ("call_544", "type-specific"),
    0x224: ("call_548", "type-specific"),
    0x228: ("call_552", "type-specific"),
    0x22C: ("call_556", "type-specific"),
    0x230: ("call_560", "type-specific"),
    0x234: ("call_564", "type-specific"),
    0x238: ("get_mana", "hero 0x00BC0700; creature 0x008A2230"),
    0x23C: ("get_hero_level", "hero 0x00BBFB90; creature 0x008A2280"),
    0x248: ("call_584", "type-specific"),
    0x28C: ("has_ability", "creature 0x008A34C0; hero 0x00A332C0"),
    0x290: ("has_skill", "hero 0x00BBFED0; creature 0x00A332C0"),
    0x294: ("has_hero_spec", "hero 0x00BBFEF0; creature 0x00A332C0"),
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

typedef int (__thiscall* GetStat)(int*);
typedef int (__thiscall* GetSkillMastery)(int*, int);
typedef int (__thiscall* HasHeroSpec)(int*, int);
typedef int* (__thiscall* GetUnitRef)(int*);
typedef int (__thiscall* GetActiveBuffById)(int*, int);
typedef int (__thiscall* HasCreatureAbility)(int*, int);
typedef float (__thiscall* GetStatFloat)(int*);
typedef int* (__thiscall* GetATBInfo)(int*);
typedef void (__thiscall* SetATBValue)(int*, float);
typedef int (__thiscall* GetCombatSpellpower)(int*, int*, int);

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
