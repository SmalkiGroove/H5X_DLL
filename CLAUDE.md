# H5X_DLL — Claude Code guide

Runtime-patching DLL for **Heroes of Might and Magic V: Tribes of the East**, H5X mod build (`H5X.exe`, 32-bit, ToE 3.1 based).
Fork of [dredknight/H5_DLL](https://github.com/dredknight/H5_DLL). `CURSOR.md` is the equivalent guide for Cursor; this file is the Claude Code one.

Binary / address conventions (always applies):
@.cursor/rules/h5x-re-binary.mdc

## Build & deploy

```bash
"/c/Program Files/Microsoft Visual Studio/18/Community/MSBuild/Current/Bin/MSBuild.exe" H5_DLL.sln -p:Configuration=Release -p:Platform=x86 -m -v:minimal -nologo
```

- Output: `Release\H5_DLL.dll`. Toolset v145 locally (CI overrides to v143), Win32 only, static CRT, buffer security checks off (`/GS-`).
- **Rebuild after every source change and fix errors until it compiles** (project rule, see `CURSOR.md`).
- Deploy: the game loads it as **`um.dll`** from `G:\__H5__\H5Xdev\bin`. `.sync-dll.ps1` renames + moves it to `%H5_Folder%\bin` — its fallback path is the vanilla Ubisoft install, so set `H5_Folder=G:\__H5__\H5Xdev` (or copy by hand).
- Runtime log: `H5.log` in the game's working dir (`bin\`), truncated on each launch. Every patched value/address is logged.
- CI: `.github/workflows/build.yml` builds on push/PR and uploads `um.dll` as an artifact.

## How the DLL gets control

`H5X.exe` imports `TrackingCount` from `um.dll` (originally Ubisoft's UbiStats telemetry). `src/dllmain.cpp` exports a replacement `TrackingCount` that does not forward to the original; its first call runs `main()` (`src/mainH5.cpp`) once (guard flag `MMH55_LOADED`, later calls are no-ops):

1. `init_patches(doc)` calls every `*_init` in a fixed order; each pushes `assembly_patch` entries into the global `assembly_patches` vector.
2. The apply loop switches on `patch_type` and writes into the live process after `VirtualProtect`.

`DllMain` does nothing. All addresses are **absolute VAs of H5X.exe** — no ASLR handling, no signature scanning.

## Repository layout

| Path | Role |
|---|---|
| `src/dllmain.cpp` | Entry point + `TrackingCount` export |
| `src/mainH5.h` / `.cpp` | `patch_type` enum, `assembly_patch` struct, all `*_init` declarations, `init_patches()` order, apply loop |
| `src/pch.h` | Precompiled header — first include of every `.cpp`; pulls in `mainH5.h`, `globals.h`, `types/{types,Functions,Artifacts,Skills}.h`, `structs/{Hero,Creature}.h` (and `CombatUnit.h` through them). `structs/Spell.h` and `types/Abilities.h` must be included explicitly |
| `src/pugixml.*` | Vendored XML parser (only for the dormant config path) |
| `src/utility_functions.cpp` | `JumpToFunction`, `ConditionalJumpToFunction`, NOP fill, byte/ptr writers, dormant XML/.pak readers |
| `src/globals.*` | Logging, `assignValueToAddress<T>`, shared float/int constants (`constf_0_5`, `consti_2`, …) used as memory operands from asm |
| `src/types/Functions.*` | Typed pointers to game functions (`count_equipped_artifact`, `has_skill`, `engine_alloc`, …) |
| `src/types/types.h` | One typedef per calling convention/signature + vtable slot aliases (`GetInt`, `GetPtr`, `SetATBValue`, …) |
| `src/types/{Artifacts,Skills,Abilities}.h` | Game ID enums (`ARTIFACT_ID`, `SKILL_ID`, `CreatureAbility`) |
| `src/structs/*.h` | RE'd vtables/structs: `CombatUnit.h` (shared hero/creature combat vtable), `Hero.h` (AdvMap hero), `Creature.h`, `Spell.h` |
| `src/patches/*.cpp` | ~125 feature modules, one per file |
| `addresses.txt` | Legacy cheat-sheet of patch sites (artifact/skill/spec checks) and stat functions |
| `.cursor/rules/*.mdc` | RE knowledge base (see below) |
| `.cursor/hooks/sync-*.py` | Regenerate rule files from `src/structs/*.h` |
| `h5x-ghidra-mcp/` | Ghidra project `H5X.rep` + MCP bridge (`bridge_mcp_ghidra.py` → `http://127.0.0.1:8080/`) |
| `Dependencies/` | libzippp/zlib/bz2 libs (linked, only used by dormant `getXmlTree`) |

## Patch module anatomy

Naming: `Mod*.cpp` = stat pipelines (attack, luck, initiative…), `HeroSpec*.cpp` = specializations, `*Fix.cpp` = bug fixes, artifact/skill/spell names otherwise. Each file opens with a comment block describing the gameplay effect.

```cpp
#include "pch.h"

// <gameplay description>

void FooFork();
int Foo_fork   = 0x00B4CA2B;   // patch site (H5X.exe VA)
int Foo_return = 0x00B4CA31;   // resume address

void Foo_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Foo_fork, 6, FooFork, 0, 0, 0 });
}

__declspec(naked) void FooFork() {
    __asm {
        // replay the overwritten instructions, new logic, then:
        jmp [Foo_return]
    }
}
```

`assembly_patch` aggregate order: `{ type, address, size, hookedFunction, value_int, value_float, value_double, value_bytes }`.

| `patch_type` | Fields read | Effect |
|---|---|---|
| `PATCH_HOOK` | address, size, hook | NOP `size` bytes, then `E9 rel32` → hook. `size ≥ 5` |
| `PATCH_CALL` | address, size, hook | Same with `E8` |
| `HOOK_JE` / `HOOK_JNE` | address, size, hook | NOP `size`, then `0F 84/85 rel32`. `size ≥ 6` |
| `PATCH_BYTE` | address, value_int | Low byte of `value_int` |
| `PATCH_INT` / `PATCH_FLOAT` | address, value_int / value_float | 4-byte write |
| `PATCH_NOP` | address, size | Fill with `0x90` |
| `PATCH_WRTE` | address, size, value_bytes | Hex string, e.g. `"B801000000C3"`; `size` = byte count |
| `PATCH_FLOAT_PTR` / `PATCH_DOUBLE_PTR` | address, value_float / value_double | Overwrites a pointer operand to point at the value stored **inside the vector element** |
| `PATCH_DOUBLE` | — | **Not implemented** in the apply loop (logs "Unsupported patch type") |

### Adding a patch — checklist

1. Create `src/patches/Foo.cpp` (pattern above). Patch sites come from Ghidra on **H5X.exe**; cross-check bytes being overwritten and choose `size` to end on an instruction boundary.
2. Add `<ClCompile Include="src\patches\Foo.cpp" />` to `H5_DLL.vcxproj` (and `H5_DLL.vcxproj.filters`) — files not listed there are silently not compiled.
3. Declare `void Foo_init(pugi::xml_document& doc);` in `src/mainH5.h` (grouped by category).
4. Call it in `init_patches()` in `src/mainH5.cpp`, in the matching group, with the `// OK` marker once tested in game.
5. Build. Then test in game and check `H5.log`.

Before hooking an address, grep `src/patches` for it (and nearby addresses) — several modules patch the same functions (e.g. the stat functions listed in `addresses.txt`), and two hooks on overlapping bytes break each other silently.

### Inline asm idioms used throughout

- **Game calls**: `call [count_equipped_artifact]` — call through the global function pointer. `__thiscall` ⇒ `this` in `ecx`, args pushed right-to-left, callee cleans.
- **Vtable calls**: `mov edx,[ecx]` / `call dword ptr [edx + 0x174]`. Slot offsets are documented in `src/structs/*.h` — reuse them rather than re-deriving.
- **Virtual-base adjust** (reach the hero interface from a combat subobject):
  `mov eax,[edi+4]` / `mov ecx,[eax+8]` / `mov edx,[ecx+edi+4]` / `lea ecx,[ecx+edi+4]` / `call [edx+0x290]` (has_skill) or `+0x294` (has_hero_spec).
- **Creature combat subobject**: creature code often addresses the outer unit at `[esi-0x144]` / `[esi-0x94]` (vtordisp, see `CombatUnit.h`).
- **Constants**: use `globals.h` `constf_*` as memory operands (`fmul [constf_0_5]`) instead of new magic floats; add new ones there if missing.
- **Return**: `jmp [X_return]` where `X_return` is a global `int` holding the VA. Hooks may have several exits — e.g. `ModLuck.cpp` jumps to `LuckToggle_return` (normal path) or `LuckToggle_end` (skip the original logic).
- **C++ helpers from asm**: put non-trivial logic in a normal C++ function declared `__fastcall` (args in `ecx`/`edx`) or `__stdcall`, and `call` it from the naked hook. Helpers shared between modules get a header next to the patch (e.g. `ElementsVirtuoso.h`, used by `ModElementalDamages.cpp` and `ModInitiative.cpp`). Typed wrappers `ICombatUnit` (`structs/CombatUnit.h`) / `ICombatHero` (`structs/Hero.h`) give C++ access to vtable slots.
- Naked functions have no prologue: preserve every register/flag the original code still needs after the return address, and replicate the instructions the jump overwrote.

## Reverse-engineering knowledge

- **Ghidra is authoritative**: project `h5x-ghidra-mcp/H5X.rep`, loaded from `G:\__H5__\H5Xdev\bin\H5X.exe`. `H5_Game.exe` in the same folder is the vanilla exe — its addresses do **not** match.
- Ghidra MCP (configured for Cursor in `.cursor/mcp.json`): decompile/disassemble by address, xrefs, rename, comments. Needs Ghidra running with the GhidraMCP plugin on port 8080.
- Naming: functions named by the project use the `func_*` prefix. Do not rename existing labels; only name `FUN_*` ones.
- The rules' section table lists **RVAs** (add `0x400000` for VAs). `.text` file offset = `VA − 0x400C00`.
- Rule files (Cursor auto-loads them; in Claude Code read the relevant one on demand):
  - `.cursor/rules/h5x-re-combat-unit.mdc` — full 171-slot `CombatUnit` vtable (hero `0x00E8499C`, creature `0x00E4FBA4`). **Auto-generated** from `src/structs/CombatUnit.h`.
  - `.cursor/rules/h5x-re-combat-atb.mdc` — ATB get/set slots, `lua_setATB` command flow, arena vtable, Lua command table.
  - `.cursor/rules/h5x-re-spell-casting.mdc` — `func_CastSpell` @ `0x0097D7B0` and named callees.
- Struct headers are the source of truth for vtables; after editing `CombatUnit.h`, `Hero.h` or `Creature.h`, regenerate the rules (Cursor does this via `afterFileEdit`; Claude Code does not):
  ```bash
  python .cursor/hooks/sync-combat-unit-rule.py
  ```
  (likewise `sync-hero-rule.py` → `h5x-re-hero-stats.mdc`, `sync-creature-rule.py` → `h5x-re-combat-creature.mdc`; those two outputs are not currently committed.)
- `addresses.txt` is older and partly vanilla-derived — verify any address from it in Ghidra before use.

## Current state (as of this analysis)

- All values are hardcoded. The `pugi::xml_document& doc` passed to every `*_init` is empty; `get*ValueFromXdb` / `getXmlTree` exist but are unused.
- Defined but **not called** from `init_patches()`: `ConjureAtb_init`, `LuaChangeAtb_init` (both commented out; `LuaChangeAtb.cpp` body is commented out too), `ArtfsetHaven4_init`, `ArtfsetRepoint_init`, `CreatureCount_init`.
- Not in `H5_DLL.vcxproj` (not compiled): `CreatureCount.cpp`, `MarketExchangeRates.cpp`.
- Init entries without the `// OK` marker (untested/in progress): `WandOfSpellTweak_init`, `Earthquake_init`.
- MinHook is referenced via NuGet (`packages.config`) but unused.

## Infrastructure quirks to know

- `JumpToFunction` / `ConditionalJumpToFunction` restore the page protection over `sizeof(len)` (4 bytes), not `len` — harmless in practice but don't rely on protection being restored.
- `ConditionalJumpToFunction` logs with `ERROR` (the Windows macro, `0`) instead of `ERRROR`; if `VirtualProtect` ever fails there, `writeLog` receives a null string.
- `*_PTR` patches make game code point into `assembly_patches` storage: never push to that vector after the apply loop has run.
- Logging writes per patch (`assignValueToAddress` logs every value) — the log is verbose by design; `logs_enabled` in `globals.cpp` toggles it.
