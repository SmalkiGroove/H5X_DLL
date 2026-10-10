#!/usr/bin/env python3
"""Regenerate .cursor/rules/h5x-re-combat-unit.mdc from src/structs/CombatUnit.h."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
COMBAT_UNIT_H = REPO_ROOT / "src/structs/CombatUnit.h"
RULE_OUT = REPO_ROOT / ".cursor/rules/h5x-re-combat-unit.mdc"

HERO_VTABLE_RE = re.compile(r"CombatHero_vtable_addr\s*=\s*(0x[0-9A-Fa-f]+)", re.I)
CREATURE_VTABLE_RE = re.compile(
    r"CombatCreature_vtable_addr\s*=\s*(0x[0-9A-Fa-f]+)", re.I
)
def enum_re(name: str) -> re.Pattern:
    return re.compile(rf"{name}\s*=\s*(0x[0-9A-Fa-f]+|\d+)", re.I)
SLOT_RE = re.compile(
    r"^\s*(?:[\w*]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)"
    r"(?:\s*\(\d+\))?\s*-\s*(?:(0x[0-9A-Fa-f]+)\s*)?(\[shared\])?\s*(.+)?$",
    re.I,
)
METHOD_RE = re.compile(r"^\s*([\w*]+)\s+(\w+)\([^;]*\)\s*\{", re.I)

SHARED_TAG = "[shared]"


def fmt_hex(value: str) -> str:
    value = value.strip()
    if not value:
        return value
    if value.lower().startswith("0x"):
        return "0x" + value[2:].upper()
    return "0x" + format(int(value), "X")


def split_struct_body(text: str, struct_name: str) -> list[str]:
    match = re.search(rf"struct {struct_name}\s*\{{([\s\S]*?)^\}};", text, re.M)
    return match.group(1).splitlines() if match else []


def parse_slots(lines: list[str]) -> list[dict]:
    slots: list[dict] = []
    for line in lines:
        match = SLOT_RE.match(line)
        if not match:
            continue
        name, offset, impl, shared_tag, note = match.groups()
        if name.startswith("call_") and not impl:
            continue
        note_text = (note or "").strip().rstrip(",")
        ghidra = ""
        if note_text:
            parts = note_text.split()
            if parts and parts[0].startswith("func_"):
                ghidra = parts[0]
            elif "func_" in note_text:
                for part in parts:
                    if part.startswith("func_"):
                        ghidra = part
                        break
        slots.append(
            {
                "name": name,
                "offset": fmt_hex(offset),
                "impl": fmt_hex(impl) if impl else "",
                "shared": bool(shared_tag),
                "ghidra": ghidra,
                "note": note_text,
            }
        )
    return slots


def parse_methods(lines: list[str]) -> list[str]:
    methods: list[str] = []
    seen: set[str] = set()
    in_iface = False
    for line in lines:
        if line.startswith("struct ICombatUnit"):
            in_iface = True
            continue
        if in_iface:
            if line.startswith("};"):
                break
            method = METHOD_RE.match(line)
            if method:
                name = method.group(2)
                if name not in seen:
                    seen.add(name)
                    methods.append(name)
    return methods


def parse_combat_unit_h(text: str) -> dict:
    hero_vtable = fmt_hex(
        HERO_VTABLE_RE.search(text).group(1) if HERO_VTABLE_RE.search(text) else "0x00E8499C"
    )
    creature_vtable = fmt_hex(
        CREATURE_VTABLE_RE.search(text).group(1)
        if CREATURE_VTABLE_RE.search(text)
        else "0x00E4FBA4"
    )
    def enum_value(name: str, default: str) -> str:
        match = enum_re(name).search(text)
        return fmt_hex(match.group(1) if match else default)

    slots = parse_slots(split_struct_body(text, "CombatUnit_vtable"))
    return {
        "hero_vtable": hero_vtable,
        "creature_vtable": creature_vtable,
        "hero_subobject": enum_value("CombatHero_subobject_offset", "0x11C"),
        "creature_subobject": enum_value("CombatCreature_subobject_offset", "0x148"),
        "hero_vbptr": enum_value("CombatHero_vbptr_offset", "0x118"),
        "creature_vbptr": enum_value("CombatCreature_vbptr_offset", "0x144"),
        "slots": slots,
        "methods": parse_methods(text.splitlines()),
    }


def shared_table(slots: list[dict]) -> str:
    return "\n".join(
        f"| `{slot['offset']}` | **{slot['name']}** | `{slot['impl']}` | {slot['note']} |"
        for slot in slots
        if slot.get("shared") and slot["impl"]
    )


def type_table(slots: list[dict]) -> str:
    return "\n".join(
        f"| `{slot['offset']}` | **{slot['name']}** | {slot['note']} |"
        for slot in slots
        if not slot.get("shared")
    )


def all_slots_table(slots: list[dict]) -> str:
    rows: list[str] = []
    for slot in sorted(slots, key=lambda s: int(s["offset"], 16)):
        if slot.get("shared"):
            impls = f"shared `{slot['impl']}` {slot['note']}"
        else:
            impls = slot["note"]
        rows.append(f"| `{slot['offset']}` | **{slot['name']}** | {impls} |")
    return "\n".join(rows)


def render_rule(data: dict) -> str:
    slots = data["slots"]
    total = len(slots)
    shared_count = sum(1 for s in slots if s.get("shared"))
    last_offset = max((s["offset"] for s in slots), key=lambda o: int(o, 16))
    method_list = "\n".join(f"- `{name}()`" for name in data["methods"])

    return f"""---
description: Shared CombatUnit vtable for heroes and creatures in battle — auto-synced from CombatUnit.h
alwaysApply: true
---

# CombatUnit — shared in-combat interface

> **Auto-generated** from `src/structs/CombatUnit.h`. Do not edit manually; run `.cursor/hooks/sync-combat-unit-rule.py` or save `CombatUnit.h` (Cursor hook).

`NWorld::CCombatHero` and `NWorld::CCombatCreature` use **parallel vtables** with the same {total}-slot layout (`CombatUnit_vtable`, offsets `0`..`{last_offset}`). **{shared_count}/{total}** slots resolve to identical implementations after the thunks. Lua and engine code refer to this concept as **CombatUnit** (`CombatUnitExist`, `CombatUnitMove`, etc.) even though RTTI uses separate class names.

| Type | Vtable | Subobject offset (RTTI) | vbptr from subobject `this` | C++ wrapper |
|---|---|---|---|---|
| **CombatHero** | `{data['hero_vtable']}` | `+{data['hero_subobject']}` | `this-{data['hero_vbptr']}` | `ICombatHero` : `ICombatUnit` |
| **CombatCreature** | `{data['creature_vtable']}` | `+{data['creature_subobject']}` | `this-{data['creature_vbptr']}` | `ICombatCreature` : `ICombatUnit` |

Entries are vtordisp thunks: `sub ecx,[ecx-4]; [sub ecx,<n>;] jmp <impl>`. The fixed adjustment `n` varies per slot (hero mostly `0x68`, creature mostly `0x94`, many slots have none; exceptions are listed in the `CombatUnit.h` header). Calling through the vtable adjusts `this` automatically.

Tables list the impl reached after the thunk with its Ghidra name (`FUN_*` = unnamed in Ghidra; tiny stubs show their instructions). Ghidra names are kept even when misleading; text after "—" describes what the code actually does.

## Shared slots (same impl on hero and creature)

| Slot | Field | Impl | Ghidra name / notes |
|---|---|---|---|
{shared_table(slots)}

## Type-specific slots (hero vs creature impl)

| Slot | Field | Hero / creature impls |
|---|---|---|
{type_table(slots)}

## All CombatUnit slots (by offset)

| Slot | Field | Impls |
|---|---|---|
{all_slots_table(slots)}

## `ICombatUnit` helpers (C++ patches)

{method_list}

Type-specific extensions: `ICombatHero` (`Hero.h`) — spellpower, skills, specs; `ICombatCreature` (`Creature.h`) — `has_ability()`.

## Patch cross-refs

- `LuaChangeAtb.cpp` — ATB via outer unit vtable `+0x188` / `+0x18C` (works for both unit types)
- `.cursor/rules/h5x-re-combat-atb.mdc` — setATB / changeATB command flow
- `.cursor/rules/h5x-re-hero-stats.mdc` — CombatHero-specific stat getters
- `.cursor/rules/h5x-re-combat-creature.mdc` — creature subobject access pattern
"""


def sync_combat_unit_rule() -> None:
    text = COMBAT_UNIT_H.read_text(encoding="utf-8")
    data = parse_combat_unit_h(text)
    RULE_OUT.parent.mkdir(parents=True, exist_ok=True)
    RULE_OUT.write_text(render_rule(data), encoding="utf-8", newline="\n")


def should_sync_from_hook(payload: dict) -> bool:
    path = payload.get("file_path", "").replace("\\", "/")
    return path.endswith("src/structs/CombatUnit.h")


def main() -> int:
    if "--hook" in sys.argv:
        raw = sys.stdin.read()
        if raw.strip():
            try:
                payload = json.loads(raw)
            except json.JSONDecodeError:
                return 0
            if not should_sync_from_hook(payload):
                return 0

    if not COMBAT_UNIT_H.is_file():
        print(f"Missing {COMBAT_UNIT_H}", file=sys.stderr)
        return 1

    sync_combat_unit_rule()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
