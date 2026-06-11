#!/usr/bin/env python3
"""Regenerate .cursor/rules/h5x-re-hero-stats.mdc from src/structs/Hero.h."""

from __future__ import annotations

import json
import re
import sys
from pathlib import Path

REPO_ROOT = Path(__file__).resolve().parents[2]
HERO_H = REPO_ROOT / "src/structs/Hero.h"
COMBAT_UNIT_H = REPO_ROOT / "src/structs/CombatUnit.h"
RULE_OUT = REPO_ROOT / ".cursor/rules/h5x-re-hero-stats.mdc"

ADVMAP_VTABLE_RE = re.compile(
    r"AdvMap hero interface \(vtable\s+(0x[0-9A-Fa-f]+)", re.I
)
COMBAT_VTABLE_RE = re.compile(r"CombatHero_vtable_addr\s*=\s*(0x[0-9A-Fa-f]+)", re.I)
SLOT_RE = re.compile(
    r"^\s*(?:[\w*]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)"
    r"(?:\s*\(\d+\))?\s*(?:-\s*(0x[0-9A-Fa-f]+),?\s*)?(.+)?$",
    re.I,
)
METHOD_RE = re.compile(r"^\s*([\w*]+)\s+(\w+)\([^;]*\)\s*\{", re.I)

PRIMARY_ADVMAP = {"get_attack", "get_defense", "get_spellpower", "get_knowledge"}
KEY_COMBAT = {
    "get_attack",
    "get_defense",
    "get_mana",
    "get_hero_level",
    "get_spell_mastery",
    "get_spellpower",
    "has_skill",
    "has_hero_spec",
}


def fmt_hex(value: str) -> str:
    value = value.strip()
    if not value:
        return value
    if value.lower().startswith("0x"):
        return "0x" + value[2:].upper()
    return "0x" + value.upper()


def parse_slots(lines: list[str]) -> list[dict]:
    slots: list[dict] = []
    for line in lines:
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
    return slots


def parse_methods(lines: list[str], struct_name: str) -> list[str]:
    methods: list[str] = []
    seen: set[str] = set()
    in_iface = False
    for line in lines:
        if line.startswith(f"struct {struct_name}"):
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


def split_struct_body(text: str, struct_name: str) -> list[str]:
    match = re.search(rf"struct {struct_name}\s*\{{([\s\S]*?)^\}};", text, re.M)
    return match.group(1).splitlines() if match else []


def parse_hero_h(text: str) -> dict:
    combat_text = COMBAT_UNIT_H.read_text(encoding="utf-8") if COMBAT_UNIT_H.is_file() else ""
    advmap_vtable = fmt_hex(
        ADVMAP_VTABLE_RE.search(text).group(1) if ADVMAP_VTABLE_RE.search(text) else "0x00E7E484"
    )
    combat_vtable = fmt_hex(
        COMBAT_VTABLE_RE.search(combat_text).group(1)
        if COMBAT_VTABLE_RE.search(combat_text)
        else "0x00E8499C"
    )

    lines = text.splitlines()
    advmap_slots = parse_slots(split_struct_body(text, "Hero_vtable"))
    combat_slots = parse_slots(split_struct_body(combat_text, "CombatUnit_vtable"))

    spellpower_slot = next((s for s in combat_slots if s["name"] == "get_spellpower"), None)
    spellpower_trap = next(
        (s for s in advmap_slots if s["name"] == "get_spellpower_for_spell"), None
    )

    return {
        "advmap_vtable": advmap_vtable,
        "combat_vtable": combat_vtable,
        "advmap_slots": advmap_slots,
        "combat_slots": combat_slots,
        "ihero_methods": parse_methods(lines, "IHero"),
        "icombat_methods": parse_methods(lines, "ICombatHero"),
        "spellpower_offset": spellpower_slot["offset"] if spellpower_slot else "0x244",
        "spellpower_impl": spellpower_slot["impl"] if spellpower_slot else "0x00A6A7E0",
        "spellpower_trap_offset": spellpower_trap["offset"] if spellpower_trap else "0x164",
        "spellpower_trap_impl": spellpower_trap["impl"] if spellpower_trap else "0x00B4B3E0",
    }


def slot_table(slots: list[dict]) -> str:
    return "\n".join(
        f"| `{slot['offset']}` | `{slot['impl']}` | **{slot['name']}** — {slot['note']} |"
        for slot in slots
        if slot["impl"]
    )


def render_rule(data: dict) -> str:
    primary_advmap = [s for s in data["advmap_slots"] if s["name"] in PRIMARY_ADVMAP]
    other_advmap = [
        s
        for s in data["advmap_slots"]
        if s["name"] not in PRIMARY_ADVMAP and s["impl"]
    ]
    key_combat = [s for s in data["combat_slots"] if s["name"] in KEY_COMBAT]
    all_combat = [s for s in data["combat_slots"] if s["impl"]]

    other_advmap_lines = "\n".join(
        f"- `{slot['offset']}` **{slot['name']}** (`{slot['impl']}`) — {slot['note']}"
        for slot in other_advmap
    )
    ihero_list = "\n".join(f"- `{name}()`" for name in data["ihero_methods"])
    icombat_list = "\n".join(f"- `{name}()`" for name in data["icombat_methods"])

    return f"""---
description: CombatHero and AdvMap hero vtables — auto-synced from Hero.h
alwaysApply: true
---

# Hero Stats — CombatHero vs AdvMap

> **Auto-generated** from `src/structs/Hero.h`. Do not edit manually; run `.cursor/hooks/sync-hero-rule.py` or save `Hero.h` (Cursor hook).

In combat (e.g. `AllDamageOutputFork` in `ModDamageOutput.cpp`), unit owner `edi` exposes two hero views:

| Object | How to get | Vtable | Use |
|---|---|---|---|
| **CombatHero** | `mov edx,[edi+4]` / `mov eax,[edx+8]` / `lea ecx,[eax+edi+4]` | `{data['combat_vtable']}` | In-combat stats, skills, specs, effective spellpower |
| **AdvMap hero** | `call [edi]` (slot 0) → `eax` | `{data['advmap_vtable']}` | Persistent totals (attack/def/spellpower/knowledge) |

AdvMap hero = `NWorld::CHero` subobject at `+0x1C` (RTTI `.?AVCHero@NWorld@@` @ `0x00F40324`). CombatHero reaches AdvMap hero via `mov ecx,[ecx-0x6C]` (`ICombatHero::hero()`).

## AdvMap hero vtable `{data['advmap_vtable']}` — primary stat getters

All `__thiscall`, **no stack args**, result in `eax` (totals incl. artifacts/week, clamped):

| Slot | Impl | Stat |
|---|---|---|
{slot_table(primary_advmap)}

Other documented AdvMap slots:

{other_advmap_lines}

## CombatHero vtable `{data['combat_vtable']}` — key slots

Shared layout: `CombatUnit_vtable` in `CombatUnit.h` (see `.cursor/rules/h5x-re-combat-unit.mdc`). Entries use vtordisp thunks (`sub ecx,[ecx-4]; sub ecx,0x68; jmp impl`).

| Slot | Impl | Meaning |
|---|---|---|
{slot_table(key_combat)}

**No knowledge getter** on CombatHero. For knowledge in combat, use AdvMap hero `+0x1C`.

### All documented CombatHero slots

| Slot | Impl | Meaning |
|---|---|---|
{slot_table(all_combat)}

## GetEffectiveSpellPower — CombatHero `{data['spellpower_offset']}`

Impl `{data['spellpower_impl']}`. Signature: `int __thiscall(ref, int flag)` — **two args, `ret 8`**.

`spellRef` (5 dwords on stack): `+0x00` spell obj (may NULL), `+0x04` spell id (0 = skip school bonuses).

Patch pattern (zeroed spellRef = no spell context):

```asm
xor eax, eax
push eax; push eax; push eax; push eax; push eax   ; 5-dword spellRef
; ecx = CombatHero subobject
push 0
lea eax, [esp+4]
push eax
call dword ptr [edx+{data['spellpower_offset']}]   ; eax = effective spellpower
add esp, 0x14
```

## The `{data['spellpower_trap_offset']}` trap (AdvMap hero)

`{data['spellpower_trap_offset']}` (`{data['spellpower_trap_impl']}`) is `GetSpellPower(spellRef*)` — **`ret 4`**, one stack arg. Calling without pushing an arg desyncs the stack and may return fixed `8`. Use `+0x18` for plain total spellpower, or CombatHero `{data['spellpower_offset']}` for combat-buffed value.

## `IHero` helpers (C++ patches)

{ihero_list}

## `ICombatHero` helpers (C++ patches)

{icombat_list}

## Patch cross-refs

- `ModDamageOutput.cpp` — Arcane Protection: `+0x290` + `{data['spellpower_offset']}`
- `ModEffectiveSpellpower.cpp` — hooks `0x00BC00D0` skill bonus region @ `0x00BC0508`
- `ModInitiative.cpp` — hooks CombatHero `+0x1BC` (`func_GetHeroInitiative`)
"""


def sync_hero_rule() -> None:
    text = HERO_H.read_text(encoding="utf-8")
    data = parse_hero_h(text)
    RULE_OUT.parent.mkdir(parents=True, exist_ok=True)
    RULE_OUT.write_text(render_rule(data), encoding="utf-8", newline="\n")


def should_sync_from_hook(payload: dict) -> bool:
    path = payload.get("file_path", "").replace("\\", "/")
    return path.endswith("src/structs/Hero.h") or path.endswith("src/structs/CombatUnit.h")


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

    if not HERO_H.is_file():
        print(f"Missing {HERO_H}", file=sys.stderr)
        return 1

    sync_hero_rule()
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
