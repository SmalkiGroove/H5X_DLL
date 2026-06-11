#!/usr/bin/env python3
"""Regenerate .cursor/rules/h5x-re-combat-creature.mdc from src/structs/Creature.h."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CREATURE_H = REPO_ROOT / "src/structs/Creature.h"
COMBAT_UNIT_H = REPO_ROOT / "src/structs/CombatUnit.h"
RULE_OUT = REPO_ROOT / ".cursor/rules/h5x-re-combat-creature.mdc"

VTABLE_RE = re.compile(r"CombatCreature_vtable_addr\s*=\s*(0x[0-9A-Fa-f]+)", re.I)
CREATURE_TYPE_SLOTS = {
    "get_unit_ref",
    "get_attack",
    "get_defense",
    "get_luck_raw",
    "get_morale_raw",
    "get_initiative",
    "get_mana",
    "has_ability",
    "has_skill",
    "has_hero_spec",
}
SLOT_RE = re.compile(
    r"^\s*(?:[\w*]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)"
    r"(?:\s*\(\d+\))?\s*-\s*(0x[0-9A-Fa-f]+|\w[\w /;]*)?\s*(\[shared\])?\s*(.+)?$",
    re.I,
)
METHOD_RE = re.compile(r"^\s*([\w*]+)\s+(\w+)\([^;]*\)\s*\{", re.I)


def fmt_hex(value: str) -> str:
    value = value.strip()
    if not value:
        return value
    if value.lower().startswith("0x"):
        return "0x" + value[2:].upper()
    return "0x" + value.upper()


def split_struct_body(text: str, struct_name: str) -> list[str]:
    match = re.search(rf"struct {struct_name}\s*\{{([\s\S]*?)^\}};", text, re.M)
    return match.group(1).splitlines() if match else []


def parse_slots(lines: list[str]) -> list[dict]:
    slots: list[dict] = []
    for line in lines:
        match = SLOT_RE.match(line)
        if not match:
            continue
        name, offset, impl, _shared, note = match.groups()
        impl_hex = ""
        if impl and impl.lower().startswith("0x"):
            impl_hex = fmt_hex(impl)
        if name.startswith("call_") and not impl_hex:
            continue
        slots.append(
            {
                "name": name,
                "offset": fmt_hex(offset),
                "impl": impl_hex,
                "note": (note or "").strip().rstrip(","),
            }
        )
    return slots


def parse_creature_h(creature_text: str, combat_text: str) -> dict:
    vtable_match = VTABLE_RE.search(combat_text)
    vtable = fmt_hex(vtable_match.group(1) if vtable_match else "0x00E4FBA4")

    all_slots = parse_slots(split_struct_body(combat_text, "CombatUnit_vtable"))
    slots = [s for s in all_slots if s["name"] in CREATURE_TYPE_SLOTS or s["name"] in {"get_atb_info", "set_atb", "get_active_buff", "get_luck", "get_morale"}]
    if not slots:
        slots = all_slots
    # ensure ATB/set_atb included
    for name in ("get_atb_info", "set_atb", "get_active_buff", "get_luck", "get_morale"):
        if not any(s["name"] == name for s in slots):
            match = next((s for s in all_slots if s["name"] == name), None)
            if match:
                slots.append(match)
    slots.sort(key=lambda s: int(s["offset"], 16))

    methods: list[str] = []
    in_iface = False
    for line in creature_text.splitlines():
        if line.startswith("struct ICombatCreature"):
            in_iface = True
            continue
        if in_iface:
            if line.startswith("};"):
                break
            method = METHOD_RE.match(line)
            if method:
                methods.append(method.group(2))

    return {"vtable": vtable, "slots": slots, "methods": methods}


def render_rule(data: dict) -> str:
    slot_rows = "\n".join(
        f"| `{slot['offset']}` | `{slot['impl']}` | **{slot['name']}** — {slot['note']} |"
        if slot["impl"]
        else f"| `{slot['offset']}` | | **{slot['name']}** — {slot['note']} |"
        for slot in data["slots"]
    )
    method_list = "\n".join(f"- `{name}()`" for name in data["methods"])

    return f"""---
description: CombatCreature vtable and ICombatCreature — auto-synced from Creature.h
alwaysApply: true
---

# CombatCreature — `src/structs/Creature.h`

> **Auto-generated** from `src/structs/Creature.h`. Do not edit manually; run `.cursor/hooks/sync-creature-rule.py` or save `Creature.h` (Cursor hook).

Combat unit in battle exposes a **CombatCreature** subobject (`NWorld::CCombatCreature`, vtordisp **`-0x144`**). Vtable **`{data['vtable']}`** — instance of shared `CombatUnit_vtable` (see `.cursor/rules/h5x-re-combat-unit.mdc`). Thunks: `sub ecx,[ecx-4]; sub ecx,0x94; jmp impl`.

## Getting `ICombatCreature*` from a unit pointer

```asm
mov eax, [unit - 0x144]          ; vbtable
mov ecx, [eax + 8]
lea ecx, [ecx + unit - 0x144]    ; ICombatCreature*
```

Creature type id on the outer unit: `[unit + 0x1C]`.

## CombatCreature vtable `{data['vtable']}` — documented slots

| Slot | Impl | Meaning |
|---|---|---|
{slot_rows}

ATB slots share implementations with heroes (`+0x188` → `[info+0x1C]`, `+0x18C` → `func_SetUnitATB`). See `.cursor/rules/h5x-re-combat-atb.mdc` for Lua/setATB flow.

`+0x290` / `+0x294` are stubs (`0x00A332C0`, always 0) — use hero `HasSkill` / `HasHeroSpec` via owner, not creature vtable.

## `ICombatCreature` helpers (C++ patches)

{method_list}

## Patch cross-refs

- `ModAbilities.cpp`, `ModLuck.cpp` — vbtable `-0x144` pattern
- `LuaChangeAtb.cpp` — ATB via unit vtable `+0x188` / `+0x18C` (creature or hero thunks)
"""


def sync_creature_rule() -> None:
    creature_text = CREATURE_H.read_text(encoding="utf-8")
    combat_text = COMBAT_UNIT_H.read_text(encoding="utf-8") if COMBAT_UNIT_H.is_file() else ""
    data = parse_creature_h(creature_text, combat_text)
    RULE_OUT.parent.mkdir(parents=True, exist_ok=True)
    RULE_OUT.write_text(render_rule(data), encoding="utf-8", newline="\n")


def should_sync_from_hook(payload: dict) -> bool:
    path = payload.get("file_path", "").replace("\\", "/")
    return path.endswith("src/structs/Creature.h") or path.endswith("src/structs/CombatUnit.h")


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

    if not CREATURE_H.is_file():
        print(f"Missing {CREATURE_H}", file=sys.stderr)
        return 1

    sync_creature_rule()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
