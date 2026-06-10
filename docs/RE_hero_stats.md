# RE notes — Hero stats in combat (CombatHero / AdvMap hero vtables)

Reverse-engineering notes on how to read hero primary stats (attack, defense,
spellpower, knowledge) from assembly patches, both through the **CombatHero**
vtable (`0x00E8499C`, in-combat values) and through the **adventure-map hero
interface** vtable (`0x00E7E484`, out-of-combat totals).

Binary: `H5X.exe` (ToE 3.1 based). ImageBase `0x400000`.
Sections: `.text` VA `0x401000` (raw `0x400`), `.rdata` VA `0xE0A000` (raw `0xA08600`),
`.data` VA `0xF1F000` (raw `0xB1C800`).

Origin: fixing the `PERK_ARCANE_PROTECTION` branch of `AllDamageOutputFork` in
`src/patches/ModDamageOutput.cpp` (damage reduction = half of hero's *total*
spellpower).

---

## 1. The two hero objects

In combat code (e.g. the damage pipeline around `0x00A5AD6E`), a unit-owner
object (`edi` in `AllDamageOutputFork`) gives access to two distinct hero views:

| Object | How to get it | Vtable | Contents |
|---|---|---|---|
| **CombatHero** subobject | `mov edx,[edi+4]` / `mov eax,[edx+8]` / `lea ecx,[eax+edi+4]` (vbtable-adjusted) | `0x00E8499C` | in-combat stats (incl. combat buffs), skills, spec |
| **AdvMap hero interface** | `mov edx,[edi]` / `mov ecx,edi` / `call [edx]` (slot 0) → `eax` | `0x00E7E484` | persistent hero: primary stats, skills, mana, level |

The AdvMap hero interface is the `NWorld::CHero` subobject at `+0x1C`
(found via RTTI: TypeDescriptor `.?AVCHero@NWorld@@` at `0x00F40324`,
COL for subobject `+0x1C` → vtable `0x00E7E484`). `NWorld::CAdvMapHero`
(TD `0x00F45C7C`) only carries abstract stubs; the concrete implementations
live in `CHero`.

CombatHero forwarders reach the AdvMap hero through `mov ecx,[ecx-0x6C]`
(pointer stored just below the vbase-adjusted subobject).

---

## 2. AdvMap hero interface vtable `0x00E7E484`

### Primary stat getters (slots +0x10..+0x1C)

All plain `__thiscall`, **no stack arguments**, result in `eax`.
They return the adventure-map *total* (base + leveling + artifact/bonus
virtual `+0x31C` + week effects − curses), clamped.

| Slot | Implementation | Stat | Notes |
|---|---|---|---|
| `+0x10` | `0x00B4CC50` | **Attack** | clamped ≥ 0 |
| `+0x14` | `0x00B4CCC0` | **Defense** | |
| `+0x18` | `0x00B4CCF0` | **Spellpower** | |
| `+0x1C` | `0x00B4CD80` | **Knowledge** | clamped ≥ 1 (named `func_GetHeroKnowledgeBase` in the Ghidra project) |

Example (from a fork where `edi` is the unit owner):

```asm
mov edx, dword ptr [edi]
mov ecx, edi
call dword ptr [edx]            // eax = AdvMap hero interface
mov edx, dword ptr [eax]
mov ecx, eax
call dword ptr [edx + 0x1C]     // eax = total knowledge, no args
```

### Other known slots

| Slot | Implementation | Meaning |
|---|---|---|
| `+0x12C` | `0x00AF8160` | get current mana points (`return [this+0x140]`) |
| `+0x130` | `0x00B4C8F0` | mana-related (used together with `+0x12C` by CombatHero `+0x238`) |
| `+0x164` | `0x00B4B3E0` | `GetSpellPower(spellRef*)` — **takes 1 arg, `ret 4`** (see §4) |
| `+0x174` | `0x00B4D150` | `HasSkill(skillId)` — returns char |
| `+0x190` | `0x00B4B5A0` | `HasHeroSpec(specId)` — same address as `has_hero_spec` in `globals.cpp` |

---

## 3. CombatHero vtable `0x00E8499C`

All entries go through `vtordisp` thunks (`sub ecx,[ecx-4]` / `sub ecx, 0xNN` /
`jmp impl`). Resolved targets below.

### Stat getters (combat values)

| Slot | Final impl | Stat | How it computes |
|---|---|---|---|
| `+0x198` | `0x00BBFE50` | Attack | `statsProvider->[+0x10]() + [this+0x3C]` (combat bonus) |
| `+0x19C` | `0x00BBFE60` | Defense | `statsProvider->[+0x14]() + [this+0x40]` + extra logic |
| `+0x1AC` | `0x00A6BD10` | Luck | |
| `+0x1B0` | `0x00A6BD50` | Morale | |
| `+0x1BC` | `0x00BC1EB0` | Initiative | |
| `+0x238` | `0x00BC0700` | **Mana** | `hero->[+0x130](x) + hero->[+0x12C]()` |
| `+0x23C` | `0x00BBFB90` | Hero level | |
| `+0x240` | `0x00A6A700` | Spell school mastery (spellRef*, flag) | |
| `+0x244` | `0x00A6A7E0` | **Effective spellpower** (spellRef*, flag) | see §4 |

`statsProvider` = pointer at `[this+0x8]` of the (thunk-adjusted) subobject; its
slots `+0x10/+0x14/+0x18/+0x1C` mirror the AdvMap stat getter layout of §2.

### Forwarders to the AdvMap hero

| Slot | Forwards to hero slot | Meaning |
|---|---|---|
| `+0x024` | `+0x284` | (1 arg) |
| `+0x034` | `+0x268` | (1 arg, returns its arg) |
| `+0x228` | `+0x0E8` | |
| `+0x290` | `+0x174` | **HasSkill(skillId)** — returns char, test `al` not `eax` |
| `+0x294` | `+0x190` | **HasHeroSpec(specId)** |

### No knowledge getter

A scan of every slot `0x000`–`0x2A8` found **no function that returns
knowledge**: nothing calls the AdvMap getter `+0x1C` or the stats provider
`+0x1C`. Consistent with the engine: in combat, knowledge is only consumed
indirectly through the mana pool (`+0x238`). To read knowledge from a combat
patch, go through the AdvMap hero interface (§2).

---

## 4. `GetEffectiveSpellPower` — CombatHero `+0x244`

`0x00E8499C +0x244` → thunk `0x00BC3020` (`vtordisp -0x68`) → `0x00A6A7E0`.

**Signature**: `int __thiscall GetEffectiveSpellPower(spellRef* ref, int flag)` —
two stack args, callee cleans (`ret 8`), result in `eax`.

`spellRef` is a small struct (the game allocates 5 dwords on the stack):

```
+0x00  spell object pointer (may be NULL)
+0x04  spell id             (0 = no spell context, skips school/mastery bonuses)
+0x08.. padding/unused for this call
```

`flag` is `0` at every game call site found.

Game call sites (precedents):

- `0x004E6570` (spell-cast event ctor): `caster->[+0x244](spellInfo, 0)` →
  stored as the cast's spellpower; `caster->[+0x240](spellInfo, 0)` → stored
  as mastery next to it.
- `0x00A5DF30`: builds `{0, 0xB, 0, 0, 0}` on the stack and calls
  `[+0x244](&struct, 0)`.

**What it includes**: base hero spellpower → + temporary combat bonus
(`[this+0xEC]`) → + buffs (Empowerment-type, ids 0x96/0x98/0x121/0x135 switch) →
+ skill bonuses (e.g. skill `0x53` +5, `0x6D` +3 — the region hooked by
`ModEffectiveSpellpower.cpp` at `0x00BC0508`) → school/week mastery bonus
(skipped when spell id = 0) → × artifact multiplier (artifact `0x146` float at
`+0x51C` of defaultstats).

Core accumulator function (not defined in Ghidra): `0x00BC00D0`,
`__thiscall (spellRef*, int)`, `ret 8` — called via the `-0xB4` subobject
slot `+0x24`.

Usage from a patch (zeroed spellRef = "no spell context"):

```asm
xor eax, eax
push eax
push eax
push eax
push eax
push eax                        // 5-dword zeroed spellRef on the stack
mov edx, dword ptr [edi + 0x4]
mov eax, dword ptr [edx + 0x8]
mov edx, dword ptr [eax + edi + 0x4]
lea ecx, dword ptr [eax + edi + 0x4] // ecx = CombatHero subobject
push 0                          // arg2 = flag
lea eax, dword ptr [esp + 0x4]
push eax                        // arg1 = &spellRef
call dword ptr [edx + 0x244]    // eax = total spellpower, callee pops args
add esp, 0x14                   // free spellRef
```

### The `+0x164` trap

AdvMap hero `+0x164` (`0x00B4B3E0`) is **not** the raw stat getter:

```c
int GetSpellPower(spellRef* ref) {        // ret 4 — one stack arg!
    if (FUN_00A34470(ref))                // "spell ignores spellpower"?
        return 8;                         // fixed pseudo-value
    return this->vtbl[+0x18]();           // real total spellpower
}
```

Calling it **without pushing an argument** desyncs the stack by 4 (the callee
still does `ret 4`) and feeds it a garbage `spellRef`, which can make it take
the `return 8` path. This was the original bug in the Arcane Protection branch
of `AllDamageOutputFork` (the stack happened to rebalance because a later
`push eax` was never popped). Use slot `+0x18` (no args) for the plain total,
or CombatHero `+0x244` for the combat-buffed value.

---

## 5. Cross-references

- `src/patches/ModDamageOutput.cpp` — `AllDamageOutputFork`, Arcane Protection
  branch uses CombatHero `+0x290` (HasSkill) + `+0x244` (effective spellpower).
- `src/patches/ModEffectiveSpellpower.cpp` — hooks inside `0x00BC00D0`'s skill
  bonus region (`0x00BC0508`).
- `src/globals.cpp` — `has_hero_spec = 0x00B4B5A0` is AdvMap hero `+0x190`.
- `docs/RE_lua_setATB.md` — PE layout, combat unit ATB interface, RTTI method.
- `addresses.txt` — `[+0x294] is hero spec`, `[+0x23C] get hero level`,
  `[+0x258] get hero class` (CombatHero slots).
