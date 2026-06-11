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
HERO_VTORDISP_RE = re.compile(r"CombatHero_vtordisp\s*=\s*(0x[0-9A-Fa-f]+|\d+)", re.I)
CREATURE_VTORDISP_RE = re.compile(
    r"CombatCreature_vtordisp\s*=\s*(0x[0-9A-Fa-f]+|\d+)", re.I
)
SLOT_RE = re.compile(
    r"^\s*(?:[\w*]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)"
    r"(?:\s*\(\d+\))?\s*-\s*(0x[0-9A-Fa-f]+)\s*(\[shared\])?\s*(.+)?$",
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
    hero_vtordisp = fmt_hex(
        HERO_VTORDISP_RE.search(text).group(1)
        if HERO_VTORDISP_RE.search(text)
        else "0x118"
    )
    creature_vtordisp = fmt_hex(
        CREATURE_VTORDISP_RE.search(text).group(1)
        if CREATURE_VTORDISP_RE.search(text)
        else "0x144"
    )
    slots = parse_slots(split_struct_body(text, "CombatUnit_vtable"))
    return {
        "hero_vtable": hero_vtable,
        "creature_vtable": creature_vtable,
        "hero_vtordisp": hero_vtordisp,
        "creature_vtordisp": creature_vtordisp,
        "slots": slots,
        "methods": parse_methods(text.splitlines()),
    }


def slot_table(slots: list[dict]) -> str:
    rows: list[str] = []
    for slot in slots:
        if not slot["impl"]:
            continue
        ghidra = f"`{slot['ghidra']}` " if slot.get("ghidra") else ""
        rows.append(
            f"| `{slot['offset']}` | `{slot['impl']}` | {ghidra}**{slot['name']}** — {slot['note']} |"
        )
    return "\n".join(rows)


def render_rule(data: dict) -> str:
    shared = [s for s in data["slots"] if s.get("shared")]
    documented = [s for s in data["slots"] if s["impl"]]
    method_list = "\n".join(f"- `{name}()`" for name in data["methods"])

    return f"""---
description: Shared CombatUnit vtable for heroes and creatures in battle — auto-synced from CombatUnit.h
alwaysApply: true
---

# CombatUnit — shared in-combat interface

> **Auto-generated** from `src/structs/CombatUnit.h`. Do not edit manually; run `.cursor/hooks/sync-combat-unit-rule.py` or save `CombatUnit.h` (Cursor hook).

`NWorld::CCombatHero` and `NWorld::CCombatCreature` use **parallel vtables** with the same 171-slot layout (`CombatUnit_vtable`, offsets `0`..`0x2A8`). Ghidra shows **92/171** slots resolving to identical implementations after vtordisp thunks. Lua and engine code refer to this concept as **CombatUnit** (`CombatUnitExist`, `CombatUnitMove`, etc.) even though RTTI uses separate class names.

| Type | Vtable | Vtordisp | C++ wrapper |
|---|---|---|---|
| **CombatHero** | `{data['hero_vtable']}` | `-{data['hero_vtordisp']}` | `ICombatHero` : `ICombatUnit` |
| **CombatCreature** | `{data['creature_vtable']}` | `-{data['creature_vtordisp']}` | `ICombatCreature` : `ICombatUnit` |

Thunk pattern: `sub ecx,[ecx-4]; sub ecx,<vtordisp>; jmp <impl>`. Calling through the vtable adjusts `this` automatically.

## Shared slots (same impl on hero and creature)

| Slot | Impl | Meaning |
|---|---|---|
{slot_table(shared)}

## All documented CombatUnit slots

| Slot | Impl | Meaning |
|---|---|---|
{slot_table(documented)}

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
