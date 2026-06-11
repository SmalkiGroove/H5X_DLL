#!/usr/bin/env python3
"""Regenerate .cursor/rules/h5x-re-combat-creature.mdc from src/structs/Creature.h."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
CREATURE_H = REPO_ROOT / "src/structs/Creature.h"
RULE_OUT = REPO_ROOT / ".cursor/rules/h5x-re-combat-creature.mdc"

VTABLE_RE = re.compile(r"vtable\s+(0x[0-9A-Fa-f]+)", re.I)
SLOT_RE = re.compile(
    r"^\s*(?:[\w*]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)"
    r"(?:\s*\(\d+\))?\s*(?:-\s*(0x[0-9A-Fa-f]+),?\s*)?(.+)?$",
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


def parse_creature_h(text: str) -> dict:
    vtable_match = VTABLE_RE.search(text)
    vtable = fmt_hex(vtable_match.group(1) if vtable_match else "0x00E4FBA4")

    slots: list[dict] = []
    for line in text.splitlines():
        match = SLOT_RE.match(line)
        if not match:
            continue
        name, offset, impl, note = match.groups()
        if name.startswith("call_") and not impl:
            continue
        slots.append(
            {
                "name": name,
                "offset": fmt_hex(offset),
                "impl": fmt_hex(impl) if impl else "",
                "note": (note or "").strip().rstrip(","),
            }
        )

    methods: list[str] = []
    in_iface = False
    for line in text.splitlines():
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

Combat unit in battle exposes a **CombatCreature** subobject (`NWorld::CCombatCreature`, vtordisp **`-0x144`**). Vtable **`{data['vtable']}`** — same slot layout as `CombatHero_vtable` (offsets `0`..`0x2A8`). Thunks: `sub ecx,[ecx-4]; sub ecx,0x94; jmp impl`.

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
    text = CREATURE_H.read_text(encoding="utf-8")
    data = parse_creature_h(text)
    RULE_OUT.parent.mkdir(parents=True, exist_ok=True)
    RULE_OUT.write_text(render_rule(data), encoding="utf-8", newline="\n")


def should_sync_from_hook(payload: dict) -> bool:
    path = payload.get("file_path", "").replace("\\", "/")
    return path.endswith("src/structs/Creature.h")


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
