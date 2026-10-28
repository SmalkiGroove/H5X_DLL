# H5X_DLL — Claude Code guide

Runtime-patching DLL for **Heroes of Might and Magic V: Tribes of the East**, H5X mod build (`H5X.exe`, 32-bit, ToE 3.1 based).
Fork of [dredknight/H5_DLL](https://github.com/dredknight/H5_DLL). `CURSOR.md` is the equivalent guide for Cursor; this file is the Claude Code one.

The mod's content (game data, Lua scripts, UI and encyclopedia, texts) is managed in `G:\__H5__\heroes-expansion`. Refer to its `CLAUDE.md` when needed.

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
| `src/types/{Artifacts,Skills,Abilities,HeroClasses,HeroSpecs,Spells}.h` | Game ID enums (`ARTIFACT_ID` + `ARTIFACT_SET_ID`, `SKILL_ID`, `CreatureAbility`, `HeroClass`, `HeroSpec`, `SPELL_ID` + `SPELL_SCHOOL_ID` + `SPELL_ELEMENT_ID`); `HeroClasses.h` / `HeroSpecs.h` / `Abilities.h` / `Spells.h` are not in `pch.h`, include them explicitly. Enum names work as inline-asm immediates (`push HERO_SPEC_STORMCALLER`) |
| `src/structs/*.h` | RE'd vtables/structs: `CombatUnit.h` (shared hero/creature combat vtable), `Hero.h` (AdvMap hero), `Creature.h` (army stack `ICreature`, combat stack `ICombatStack` = NWorld::ICombatCreature at +0xA0), `Spell.h` |
| `src/patches/*.cpp` | ~125 feature modules, one per file |
| `addresses.txt` | Legacy cheat-sheet of patch sites (artifact/skill/spec checks) and stat functions |
| `.cursor/rules/*.mdc` | RE knowledge base (see below) |
| `.cursor/hooks/sync-*.py` | Regenerate rule files from `src/structs/*.h` |
| `h5x-ghidra-mcp/` | Ghidra project `H5X.rep` + MCP bridge (`bridge_mcp_ghidra.py` → `http://127.0.0.1:8080/`) |
| `Dependencies/` | libzippp/zlib/bz2 libs (linked, only used by dormant `getXmlTree`) |
| `tools/h5m/h5m.py` | Adventure map reader (`summary`, `terrain`, `regions` → region PNG + gate list), see `h5x-map-format.mdc` |
| `tools/h5index/h5index.py` | DB object index (`index.bin`) reader / updater (`stats`, `list`, `diff`, `missing`, `update`), see `h5x-re-index-bin.mdc` |

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
- **Combat subobjects**: from the CombatUnit subobject `this` (vtable at `+0x11C` hero / `+0x148` creature in the complete object), the object's vbptr is at `this-0x118` / `this-0x144` — hence `mov eax,[esi-0x144]; mov ecx,[eax+8]` in creature hooks. `0x68` / `0x94` are the usual vtable-thunk adjustments (`sub ecx,[ecx-4]; sub ecx,0x94`). Constants: `Combat*_subobject_offset` / `Combat*_vbptr_offset` in `CombatUnit.h`.
- **Constants**: use `globals.h` `constf_*` as memory operands (`fmul [constf_0_5]`) instead of new magic floats; add new ones there if missing.
- **Return**: `jmp [X_return]` where `X_return` is a global `int` holding the VA. Hooks may have several exits — e.g. `ModLuck.cpp` jumps to `LuckToggle_return` (normal path) or `LuckToggle_end` (skip the original logic).
- **C++ helpers from asm**: put non-trivial logic in a normal C++ function declared `__fastcall` (args in `ecx`/`edx`) or `__stdcall`, and `call` it from the naked hook. Helpers shared between modules get a header next to the patch (e.g. `ElementsVirtuoso.h`, used by `ModElementalDamages.cpp` and `ModInitiative.cpp`). Typed wrappers `ICombatUnit` (`structs/CombatUnit.h`) / `ICombatHero` (`structs/Hero.h`) give C++ access to vtable slots.
- Naked functions have no prologue: preserve every register/flag the original code still needs after the return address, and replicate the instructions the jump overwrote.

## Reverse-engineering knowledge

- **Ghidra is authoritative**: project `h5x-ghidra-mcp/H5X.rep`, loaded from `G:\__H5__\H5Xdev\bin\H5X.exe`. `H5_Game.exe` in the same folder is the vanilla exe — its addresses do **not** match.
- Ghidra MCP (`mcp__ghidra__*` tools, configured in `.mcp.json` for Claude Code and `.cursor/mcp.json` for Cursor): decompile/disassemble by address, xrefs, rename, comments. Needs Ghidra running with the GhidraMCP plugin on port 8080; MCP servers load at session start. Raw bytes (vtables, thunks, constants) can be read from `H5X.exe` directly with the section table above.
- Naming: functions named by the project use the `func_*` prefix.
- The rules' section table lists **RVAs** (add `0x400000` for VAs). `.text` file offset = `VA − 0x400C00`.
- Rule files (Cursor auto-loads them; in Claude Code read the relevant one on demand):
  - `.cursor/rules/h5x-re-combat-unit.mdc` — full 181-slot `CombatUnit` vtable (hero `0x00E8499C`, creature `0x00E4FBA4`), impls after thunks with Ghidra names. **Auto-generated** from `src/structs/CombatUnit.h`.
  - `.cursor/rules/h5x-re-combat-atb.mdc` — ATB slots, `func_SetUnitATB` behaviour (caps at 1.25, no lower clamp), `lua_setATB` command flow, arena vtable, combat Lua function table (don't overwrite its terminator), engine → Lua callbacks (`DoPrepare`, `UnitMove`, `UnitDeath`, H5X `UnitAttack`).
  - `.cursor/rules/h5x-re-spell-casting.mdc` — `func_CastSpell` @ `0x0097D7B0`: spell-ID dispatch table, named callees, event sizes, post-cast ATB.
  - `.cursor/rules/h5x-re-artifact-sets.mdc` — hero set piece counts (`IHero` +0x328) and class (+0x258), set ID list with exe call sites, the 10 → 40 set expansion, Haven set hooks.
  - `.cursor/rules/h5x-re-save-format.mdc` — `.sav` layout and saver encoding (varints, tagged fields, object chunks, object table), save/load entry points, single-player interface stack roots, window and screen fields as saved (`DocWindowSaveFix`).
  - `.cursor/rules/h5x-map-format.mdc` — `.h5m` maps: archive layout, `map.xdb` objects / players / footprints, `GroundTerrain.bin` encoding, coordinates (y up, counter-clockwise rotation), H5X_Stadium layout.
  - `.cursor/rules/h5x-re-index-bin.mdc` — `index.bin` (DB objects by class `TypeID`): layout, type IDs from `types.xml`, listing rule, exe loader and per-VFS merge, updating it with the mod's content.
  - `.cursor/rules/h5x-re-neutral-heroes.mdc` — design notes for hero-led neutral stacks (not implemented): why map heroes need an owner, monster fight setup and its defender hero slot (guarded loader `0x00856460`), Lua `StartCombat` ambush, prison heroes as the editor-side hero configuration, open questions.
  - `.cursor/rules/h5x-re-lua-engine.mdc` — Lua 4.0 script engine: contexts (`combat`, `advmap`, `town`, `tutorial`) and their function tables, `CLuaScriptEngine`, C function conventions and argument format, core functions (`sleep`, `startThread`, `doFile`, …), engine → Lua calls (waited `Callback`, triggers), single player vs multiplayer gates, H5X Lua extensions. Script-side API: `heroes-expansion/.cursor/rules/lua.md`.
  - `.cursor/rules/h5x-re-rmg.mdc` — random map generator: pipeline steps and per-zone placers (`func_RMG_*`, `func_RMGZone_*`), template / parameter data, RMG random generator, treasure blocks (the only RMG artifacts: pool from `Artifacts.xdb`, value window, placement that fails silently), guard strength by monster level, the three MMH55 RMG exe patches not yet ported.
- Struct headers are the source of truth for vtables; after editing `CombatUnit.h`, `Hero.h` or `Creature.h`, regenerate the rules (Cursor does this via `afterFileEdit`; Claude Code does not):
  ```bash
  python .cursor/hooks/sync-combat-unit-rule.py
  ```
  (likewise `sync-hero-rule.py` → `h5x-re-hero-stats.mdc`, `sync-creature-rule.py` → `h5x-re-combat-creature.mdc`; those two outputs are not currently committed.)
- `addresses.txt` is older and partly vanilla-derived — verify any address from it in Ghidra before use.

## Current state (as of this analysis)

- All values are hardcoded. The `pugi::xml_document& doc` passed to every `*_init` is empty; `get*ValueFromXdb` / `getXmlTree` exist but are unused.
- Defined but **not called** from `init_patches()`: `ConjureAtb_init` (commented out), `ArtfsetRepoint_init` (`ArtfsetExpand.cpp`, no call), `CreatureCount_init`.
- Not in `H5_DLL.vcxproj` (not compiled): `CreatureCount.cpp`.
- Init entries without the `// OK` marker (untested/in progress): `RmgArtifactLog_init` (`RmgArtifactLog.cpp`, diagnostic: logs each RMG treasure block artifact placement and whether `func_RMG_PlaceObject` created it, H5.log "RmgArtifact: ...", see `h5x-re-rmg.mdc`; remove once the empty relic tiles are understood), `RmgGuardLog_init` (`RmgGuardLog.cpp`, diagnostic: wraps `func_MonsterSetter_MakeGuard` `0x00BA7690` and logs every RMG guard, the creature picked and whether a monster was created, H5.log "RmgGuard: ..."), `RmgTeams_init` (**disabled in `mainH5.cpp`**: the second in-game test still gave no teams; `RmgTeams.cpp`: templates ending with the mod's `<Teams>` element give the generated map `CustomTeams` and each player's `Team`; read hook at the end of the template serializer `0x004A2465`, applied before `FUN_00909ce0` at `0x0090BD1C` in `func_RMG_SaveMap`; first test: the read hook never ran (template loaded before the DLL patched), so the save step now falls back to reading the template `.xdb` from the game paks with libzippp / pugixml; H5.log "RmgTeams: ...", see `h5x-re-rmg.mdc`), `LuaRandom_init` (`LuaRandom.cpp`: Lua `random(a, b)` inclusive, in advmap (vanilla `random` entry repointed, script owner's generator) and combat (H5X combat table, per-combat copy of the combat generator); the mod's Lua `random` in `common.lua` must be removed or it overrides it), `LuaCastAreaSpell_init` (`LuaCastAreaSpell.cpp`: combat `UnitCastAreaSpell` accepts -1 for x / y, the matching cells are tried in random order through the vanilla cast `0x00566940` (call at `0x0056A027`); no valid cell = no cast, no error), `LuaCombatFunctions_init` (`LuaCombatFunctions.cpp`: H5X combat function table replacing the combat context's cinematic table pointer in the context registry; combat ports of `GetHeroLevel`, `GetHeroStat`, `GetHeroSkillMastery`, `HasArtefact`, `GetArtifactSetItemsCount`, `KnowHeroSpell` for the two combat heroes, by script name or `attacker-hero` / `defender-hero`; shared Lua helpers in `LuaScript.h` / `.cpp`), `Scripts_init` (`EnableScripts.cpp`: patches only the combat script gate, a hook loading the combat script in network multiplayer on the human attacker's machine only (else the human defender's), local player captured from the adventure screen at `0x00608D08`; the advmap, trigger event, town and flyby gates are vanilla; H5.log "CombatScriptHost: ..."), `WandOfSpellTweak_init`, `ThrillOfCombat_init` (`ThrillOfCombat.cpp`: a hero with skill `SPEC_THRILL_OF_COMBAT` (24) rolls good morale after attacking, not after spells or abilities: the creature roll `0x00A6BA00` and the good morale effect `+0x164` run after the hero attack ATB reset, call at `0x008CFB27` in `func_ComputeHeroAttackEffects`), `CombatCamera_init` (combat camera pitch range and min zoom distance), `NetworkInterval_init` (multiplayer send interval 0.05 → 0.02 s), `RemoveMapScriptCheck_init` (`advmap-startup.lua` / `createAdvmapAliases()` also run on maps without a map script), `LuaUnitAttack_init` (Lua `UnitAttack(attacker, defender)` combat callback after creature, war machine and hero attacks, see `h5x-re-combat-atb.mdc`), `PerkCreatureGiftFix_init` (Battle Commander / Goblin Support creatures join the matching stack, else the base creature), `ArtfsetAcademy_init` (Academy set rework: start / cast ATB, caster mana and resistance hooks; spellpower, hero cast ATB and damage reduction go through `ModEffectiveSpellpower`, `ModCasterCreatures`, `ModAtbReset` and `ModDamageOutput`), `GoldenGoose_init` (artifact 161: lucky strikes +0.33 damage multiplier, entry hook on `func_GetGoodLuckMultiplier` `0x00A6A680`), `TarotDeck_init` (artifact 87 gives the Lucky Caster perk effect: check at `0x00983AD6` in the hero spell cast function `0x00983790`, plus +1 luck for that roll only through a flag read in the hero luck roll at `0x00A6B841`), `MercenaryBand_init` (artifact 57: creature min damage raised to the min / max average, entry hook on `func_GetCreatureMinDamage` `0x004BBFB0`), `NoRetreat_init` (artifact 7: every creature in the combat has 0 speed when either hero carries it, end of `func_CombatCreature_ComputeSpeedModifiers` `0x008A20DF`), `NagaCommandersHelmet_init` (artifact 105: +1 diplomacy level in the neutral join check `0x008E1AD0`, hook at `0x008E1C2A`; also -0.25 mood modifier per diplomacy level instead of -0.2 for every hero, operand `0x008DA247`), `ElrathsDivineShield_init` (artifact 199, Elrath's Divine Shield: good morale ATB +0.2 over DefaultStats `+0xAD4`, hook at `0x00A6D0C6` in the good morale effect `0x00A6CF90`), `DragonFlameTongue_init` (artifact 43: creature attacks +15% bonus fire damage, replaces the Fiery Wrath block of `func_GetAdditionalFireDmg` at `0x00A5EDDE`), `StartingEffects_init` (`ModStartingEffects.cpp`: combat start hook on every creature, Frost set 4 freeze, Spirit set 5 permanent Sorrow and the Shackles of the Last Man effect icon, which needs the spell count exe patch `h5m_spells-count.yml`).
- `RmgGuardPoolFix_init` (tested 2026-10-27): the RMG guard creature pick `0x00BA6910` skips creatures without a `MonsterShared` link (hook at `0x00BA69EE`); before, Darkstorm 1–50, Druid of the Council and Avatar of Death could be picked and left treasures unguarded. Test map: 500 guards placed, none missing (`RmgGuardLog`).
- `DocWindowSaveFix_init` (tested 2026-10-17): single-player saves leave out the encyclopedia window tree `DocRootWindow`, which is re-created from XDB after a load (H5.log: "DocWindowSaveFix: ..."). A Stadium save went from 11.5 MB to 4 MB. The file header documents the save/load mechanism and the window layout offsets (children vector, children map, screen button-group map, Shared children).
- Patches without an `_init`, called from other modules' hooks (`Artfset.h`): `ArtfsetFrost.cpp` (Frost set 4: freeze through `ModStartingEffects`, ice resistance bypass through `ModSpellPenetration.cpp`, untested), `ArtfsetBandit.cpp` (Bandit set 5: +1 speed through `ModSpeed`, No Enemy Retaliation through the shared ability hook, untested), `ArtfsetSpirit.cpp` (Spirit set 5: permanent Sorrow through `ModStartingEffects`, −20% attack / defense through `ModAttack` / `ModDefense`, untested), `ArtfsetNecro.cpp` (Necropolis set 6-piece abilities, shared ability hook in `ModAbilities.cpp`, untested), `ArtfsetOrcs.cpp` (Orcs set: Beat-up, speed, morale, starting rage through the shared ability hook, `ModSpeed`, `ModMorale` and `ModStartingRage`, untested). The tested set inits also route part of their effects through shared hooks: `ArtfsetDwarven_init` (formations / abilities / spell proof via `OffensiveDefensiveFormation`, the shared ability hook and `ModSpellProof`; the 6-piece +5 HP through `ModHealth`, added since the test), `ArtfsetDungeon_init` (via `ModEffectiveSpellpower`, `ModInitiative`, `UnholyWordImmunitiesFix` and the shared ability hook), `ArtfsetInferno_init` (fire resistance bypass now in `ModSpellPenetration.cpp`).
- Marked OK but changed since the test: `CreatureAbilities_init` (Captain's Hook grants Wound), `SpellImmune_init` (`ModSpellImmune.cpp` rewritten in C++; the Forgotten Ring / Holy Word check at `0x0097CC2C` is now a hook, the old `PATCH_BYTE` pushed a sign-extended -72 and never matched; Forgetfulness / Berserk immunity moved from Plunderer Vest (128, a typo) to Celestial Justicar Helmet (50), the vanilla cases now run when the DLL artifact is missing), `StartingRage_init` (`ModStartingRage.cpp`, rewritten in C++ with the Orcs set rage), `ArtfsetExpand_init` (set-count vector sized 40, bounds-checked getter, equip hook at `0x00B5A4AA` teaching the Sylvan / Dungeon 4-piece spells), `SpellPenetration_init` (Chaos Shroud: 66% spell penetration next to Genji's Silksword, artifact part in C++; now also hosts the Inferno / Frost resistance bypass at `0x0097D2FB`), `AtbReset_init` (rewritten for the Academy hero cast ATB), `CasterCreatures_init` / `Health_init` / `DamageOutput_init` / `EffectiveSpellpower_init` (Academy set rework).
- MinHook is referenced via NuGet (`packages.config`) but unused.

## Infrastructure quirks to know

- `JumpToFunction` / `ConditionalJumpToFunction` restore the page protection over `sizeof(len)` (4 bytes), not `len` — harmless in practice but don't rely on protection being restored.
- `ConditionalJumpToFunction` logs with `ERROR` (the Windows macro, `0`) instead of `ERRROR`; if `VirtualProtect` ever fails there, `writeLog` receives a null string.
- `*_PTR` patches make game code point into `assembly_patches` storage: never push to that vector after the apply loop has run.
- Logging writes per patch (`assignValueToAddress` logs every value) — the log is verbose by design; `logs_enabled` in `globals.cpp` toggles it.
