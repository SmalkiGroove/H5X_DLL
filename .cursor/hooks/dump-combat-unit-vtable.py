#!/usr/bin/env python3
"""Dump CombatHero vs CombatCreature vtable slots from H5X.exe (thunk-resolved impls)."""

from __future__ import annotations

import struct
import sys
from pathlib import Path

HERO_VTABLE = 0x00E8499C
CREATURE_VTABLE = 0x00E4FBA4
SLOT_COUNT = 0x2AC  # 171 slots, 4 bytes each

DEFAULT_EXE = Path(r"G:\__H5__\H5Xdev\bin\H5X.exe")


def load_pe_sections(data: bytes) -> tuple[int, list[tuple[int, int, int]]]:
    e_lfanew = struct.unpack_from("<I", data, 0x3C)[0]
    image_base = struct.unpack_from("<I", data, e_lfanew + 24 + 28)[0]
    num_sections = struct.unpack_from("<H", data, e_lfanew + 6)[0]
    opt_hdr = e_lfanew + 24
    sect_off = opt_hdr + struct.unpack_from("<H", data, e_lfanew + 20)[0]
    sections: list[tuple[int, int, int]] = []
    for i in range(num_sections):
        o = sect_off + i * 40
        vsize, vaddr, rsize, roff = struct.unpack_from("<IIII", data, o + 8)
        sections.append((vaddr, roff, max(vsize, rsize)))
    return image_base, sections


def va_to_offset(va: int, image_base: int, sections: list[tuple[int, int, int]]) -> int | None:
    rva = va - image_base
    for vaddr, roff, size in sections:
        if vaddr <= rva < vaddr + size:
            return roff + (rva - vaddr)
    return None


def read_bytes(data: bytes, va: int, image_base: int, sections: list[tuple[int, int, int]], n: int) -> bytes:
    off = va_to_offset(va, image_base, sections)
    if off is None:
        return b""
    return data[off : off + n]


def read_u32(data: bytes, va: int, image_base: int, sections: list[tuple[int, int, int]]) -> int:
    raw = read_bytes(data, va, image_base, sections, 4)
    if len(raw) < 4:
        return 0
    return struct.unpack_from("<I", raw)[0]


def thunk_target(ptr: int, data: bytes, image_base: int, sections: list[tuple[int, int, int]]) -> int:
    b = read_bytes(data, ptr, image_base, sections, 20)
    if len(b) < 5:
        return ptr
    if b[:3] == bytes([0x2B, 0x49, 0xFC]):
        if b[3] == 0xE9:
            rel = struct.unpack_from("<i", b, 4)[0]
            return ptr + 8 + rel
        if b[3] == 0x83 and b[4] == 0xE9 and len(b) >= 11:
            rel = struct.unpack_from("<i", b, 7)[0]
            return ptr + 11 + rel
        if b[3] == 0x81 and b[4] == 0xE9 and len(b) >= 14:
            # Creature vtordisp: sub ecx,0x94; jmp +2; [pad]; add ecx,-0x94; jmp impl
            if b[9] == 0xE9 and struct.unpack_from("<i", b, 10)[0] == 2:
                return ptr + 16
            rel = struct.unpack_from("<i", b, 10)[0]
            return ptr + 14 + rel
    if b[0] == 0x81 and b[1] == 0xC1 and len(b) >= 11:
        rel = struct.unpack_from("<i", b, 7)[0]
        return ptr + 11 + rel
    if b[0] == 0xE9:
        rel = struct.unpack_from("<i", b, 1)[0]
        return ptr + 5 + rel
    return ptr


def resolve_vtable_impl(ptr: int, data: bytes, image_base: int, sections: list[tuple[int, int, int]]) -> int:
    seen: set[int] = set()
    for _ in range(8):
        if ptr in seen or ptr < 0x401000:
            break
        seen.add(ptr)
        nxt = thunk_target(ptr, data, image_base, sections)
        if nxt == ptr:
            break
        ptr = nxt
    return ptr


def dump_vtable(exe_path: Path) -> list[tuple[int, int, int, bool]]:
    data = exe_path.read_bytes()
    image_base, sections = load_pe_sections(data)
    rows: list[tuple[int, int, int, bool]] = []
    for slot in range(0, SLOT_COUNT, 4):
        hero_ptr = read_u32(data, HERO_VTABLE + slot, image_base, sections)
        crea_ptr = read_u32(data, CREATURE_VTABLE + slot, image_base, sections)
        hero_impl = resolve_vtable_impl(hero_ptr, data, image_base, sections)
        crea_impl = resolve_vtable_impl(crea_ptr, data, image_base, sections)
        rows.append((slot, hero_impl, crea_impl, hero_impl == crea_impl))
    return rows


def main() -> int:
    exe = Path(sys.argv[1]) if len(sys.argv) > 1 else DEFAULT_EXE
    if not exe.is_file():
        print(f"ERROR: {exe} not found", file=sys.stderr)
        return 1

    rows = dump_vtable(exe)
    shared = sum(1 for *_, same in rows if same)
    print(f"# {exe.name}: {len(rows)} slots, {shared} shared, {len(rows) - shared} type-specific")
    print("slot,hero_impl,creature_impl,shared")
    for slot, hi, ci, same in rows:
        print(f"0x{slot:03X},0x{hi:08X},0x{ci:08X},{same}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
