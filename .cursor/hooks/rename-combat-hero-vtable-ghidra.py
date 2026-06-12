#!/usr/bin/env python3
"""Sync CombatHero vtable pointer labels into Ghidra from CombatUnit.h.

Pointer labels follow func_* Ghidra names with ref_ prefix (CombatHero slot):
  func_CombatUnit_NotifyOwner -> ref_CombatUnit_NotifyOwner
  func_HasHeroSpec            -> ref_HasHeroSpec

1. HTTP renameData (Ghidra MCP @ :8080) — quick symbol pass.
2. Emits ghidra-RenameCombatHeroVtableFields.py — run in Script Manager to rename
   struct_CombatHero_vtable components (required for Listing to show ref_* instead
   of LAB_<thunk>).

Usage:
  python .cursor/hooks/rename-combat-hero-vtable-ghidra.py
  python .cursor/hooks/rename-combat-hero-vtable-ghidra.py --labels-only
  python .cursor/hooks/rename-combat-hero-vtable-ghidra.py --emit-script-only
"""

from __future__ import annotations

import argparse
import re
import sys
import time
from pathlib import Path

import requests

REPO = Path(__file__).resolve().parents[2]
COMBAT_UNIT_H = REPO / "src/structs/CombatUnit.h"
GHIDRA_SCRIPT = Path(__file__).resolve().parent / "ghidra-RenameCombatHeroVtableFields.py"
GHIDRA_URL = "http://127.0.0.1:8080/"
VTABLE_ADDR = 0x00E8499C

SLOT_RE = re.compile(
    r"^\s*(?:[\w*:<>]+\s+)?(\w+);\s*//\s*(0x[0-9A-Fa-f]+)(?:\s*\(\d+\))?\s*-\s*(.+)$",
    re.I,
)
HERO_FUNC_RE = re.compile(r"hero\s+0x[0-9A-Fa-f]+\s+(func_\w+)", re.I)
SHARED_FUNC_RE = re.compile(r"\[shared\]\s*(func_\w+)", re.I)
ANY_FUNC_RE = re.compile(r"func_\w+")


def hero_func_name(note: str) -> str:
    """Resolve hero-side func_* for a CombatHero vtable slot comment tail."""
    m = HERO_FUNC_RE.search(note)
    if m:
        return m.group(1)
    m = SHARED_FUNC_RE.search(note)
    if m:
        return m.group(1)
    funcs = ANY_FUNC_RE.findall(note)
    if not funcs:
        raise ValueError(f"no func_ name in slot comment: {note!r}")
    return funcs[0]


def func_to_ref(func: str) -> str:
    return "ref_" + func[5:] if func.startswith("func_") else "ref_" + func


def parse_slots(text: str) -> list[tuple[int, str, str]]:
    """Return (offset, snake_field, ref_label) for each vtable slot."""
    body = text.split("struct CombatUnit_vtable {", 1)[1].split("};", 1)[0]
    slots: list[tuple[int, str, str]] = []
    for line in body.splitlines():
        m = SLOT_RE.match(line)
        if not m:
            continue
        field = m.group(1)
        off = int(m.group(2), 16)
        ref = func_to_ref(hero_func_name(m.group(3)))
        slots.append((off, field, ref))
    return slots


def emit_ghidra_script(slots: list[tuple[int, str, str]]) -> str:
    lines = [
        "# Auto-generated from src/structs/CombatUnit.h — do not edit by hand.",
        "# @category H5X",
        "# @menupath Tools.H5X.Rename CombatHero Vtable Fields",
        "",
        "from ghidra.program.model.data import PointerDataType",
        "from ghidra.program.model.symbol import SourceType",
        "",
        f"VTABLE_ADDR = toAddr(0x{VTABLE_ADDR:08X})",
        'STRUCT_NAMES = ("struct_CombatHero_vtable", "CombatUnit_vtable", "CombatHero_vtable")',
        "",
        "# (ref_label, offset)",
        "REF_FIELDS = [",
    ]
    for off, _field, ref in slots:
        lines.append(f'    ("{ref}", 0x{off:03X}),')
    lines.extend(
        [
            "]",
            "",
            "def find_struct():",
            "    dtm = currentProgram.getDataTypeManager()",
            "    for name in STRUCT_NAMES:",
            "        dt = dtm.getDataType('/' + name)",
            "        if dt is not None:",
            "            return dt",
            "    it = dtm.getAllDataTypes()",
            "    while it.hasNext():",
            "        dt = it.next()",
            "        if dt.getName() in STRUCT_NAMES:",
            "            return dt",
            "    return None",
            "",
            "struct = find_struct()",
            "if struct is None:",
            '    print("ERROR: struct_CombatHero_vtable not found — create/apply struct first")',
            "else:",
            "    ptr = PointerDataType()",
            "    for ref_label, off in REF_FIELDS:",
            "        comp = struct.getComponentAt(off)",
            "        comment = comp.getComment() if comp is not None else None",
            "        struct.replaceAt(off, ptr, ref_label, comment)",
            '    print("Renamed %d struct components on %s" % (len(REF_FIELDS), struct.getName()))',
            "",
            "listing = currentProgram.getListing()",
            "symtab = currentProgram.getSymbolTable()",
            "end = VTABLE_ADDR.add(len(REF_FIELDS) * 4 - 4)",
            "listing.clearCodeUnits(VTABLE_ADDR, end, False)",
            "if struct is not None:",
            "    listing.createData(VTABLE_ADDR, struct)",
            '    print("Re-applied %s at %s" % (struct.getName(), VTABLE_ADDR))',
            "",
            "for ref_label, off in REF_FIELDS:",
            "    addr = VTABLE_ADDR.add(off)",
            "    sym = symtab.getPrimarySymbol(addr)",
            "    if sym is None:",
            "        symtab.createLabel(addr, ref_label, SourceType.USER_DEFINED)",
            "    else:",
            "        sym.setName(ref_label, SourceType.USER_DEFINED)",
            "",
            'print("Done: %d ref_ pointer labels @ 0x%08X" % (len(REF_FIELDS), 0x'
            + f"{VTABLE_ADDR:X}"
            + "))",
            "",
        ]
    )
    return "\n".join(lines)


def rename_data(address: int, name: str) -> str:
    r = requests.post(
        GHIDRA_URL.rstrip("/") + "/renameData",
        data={"address": f"0x{address:08X}", "newName": name},
        timeout=30,
    )
    return r.text.strip() if r.ok else f"Error {r.status_code}: {r.text.strip()}"


def apply_labels(slots: list[tuple[int, str, str]]) -> tuple[int, int]:
    ok = fail = 0
    for off, field, ref in slots:
        result = rename_data(VTABLE_ADDR + off, ref)
        if result.startswith("Error") or "failed" in result.lower():
            print(f"FAIL +0x{off:03X} {field} -> {ref}: {result}")
            fail += 1
        else:
            ok += 1
        time.sleep(0.02)
    return ok, fail


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("--labels-only", action="store_true")
    parser.add_argument("--emit-script-only", action="store_true")
    args = parser.parse_args()

    text = COMBAT_UNIT_H.read_text(encoding="utf-8")
    slots = parse_slots(text)
    if len(slots) != 171:
        print(f"Expected 171 slots, got {len(slots)}", file=sys.stderr)
        return 1

    # Spot-check expected naming
    by_off = {off: ref for off, _f, ref in slots}
    assert by_off[0x010] == "ref_CombatUnit_NotifyOwner"
    assert by_off[0x294] == "ref_HasHeroSpec"

    GHIDRA_SCRIPT.write_text(emit_ghidra_script(slots), encoding="utf-8", newline="\n")
    print(f"Wrote {GHIDRA_SCRIPT.relative_to(REPO)}")

    if args.emit_script_only:
        return 0

    print(
        "Run Script Manager -> H5X -> Rename CombatHero Vtable Fields if Listing\n"
        "still shows LAB_<thunk> (struct component names drive the display)."
    )

    ok, fail = apply_labels(slots)
    print(f"HTTP labels: {ok} ok, {fail} failed")
    return 0 if fail == 0 else 1


if __name__ == "__main__":
    raise SystemExit(main())
