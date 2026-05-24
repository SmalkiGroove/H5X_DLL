# H5X_DLL — Project Context

## What This Is

A Win32 DLL mod for **Heroes of Might and Magic V: Tribes of the East** (H5 ToE, 32-bit executable). It is built as a Visual Studio project (`H5_DLL.sln` / `H5_DLL.vcxproj`) and deployed as `um.dll` into the game's `bin` folder.

This is a fork of [dredknight/H5_DLL](https://github.com/dredknight/H5_DLL), extended for the **H5X mod**.

## How the Hook Works

The game loads `um.dll` and calls its exported `TrackingCount` function — a hook point that normally belongs to Ubisoft's `UbiStats2.dll` telemetry SDK. This project replaces that DLL without forwarding to the original. On the **first call** to `TrackingCount`, `main()` runs once: it queues all patches via `init_patches()`, applies them in a loop using `VirtualProtect`, then sets a guard flag so subsequent calls are no-ops.

All patching is **runtime memory patching**: the DLL writes jumps, NOPs, bytes, and integers directly into the game process's address space at hardcoded absolute addresses in the ToE executable.

## Project Structure

```
H5X_DLL/
├── src/
│   ├── dllmain.cpp             # DLL entry point + TrackingCount export
│   ├── mainH5.cpp / .h         # Patch orchestration: init_patches(), apply loop, patch_type enum
│   ├── globals.cpp / .h        # Game function pointers, logging, memory helper templates
│   ├── utility_functions.cpp   # Low-level patch helpers (JMP/CALL writers, NOP, byte writes)
│   ├── pch.cpp / pch.h         # Precompiled header
│   ├── pugixml.cpp / .hpp      # Vendored XML parser
│   ├── patches/                # ~120 feature modules (one .cpp per patch/tweak)
│   ├── types/                  # Enums and type definitions (ARTIFACT_ID, SKILL_ID, vtables)
│   └── structs/                # Reverse-engineered game struct layouts (Hero, etc.)
├── H5_DLL.def                  # Module definition file (EXPORTS block is empty)
├── addresses.txt               # Developer reference: game exe RVAs for functions/structs
├── .sync-dll.ps1               # Deploy script: renames DLL to um.dll, copies to game bin/
└── Dependencies/               # libzippp header (for .pak archive reading)
```

## Patch Architecture

### Data flow

1. Each feature lives in `src/patches/*.cpp` and exposes a single `FeatureName_init(pugi::xml_document&)` function.
2. `init_patches()` in `mainH5.cpp` calls every `*_init` in a fixed order, which pushes entries into the global `std::vector<assembly_patch> assembly_patches`.
3. `main()` iterates the vector and applies each patch.

### `assembly_patch` struct and `patch_type` enum

Defined in `mainH5.h`. Each patch entry carries:
- A type (see below)
- A target address in game memory
- A size (bytes to overwrite / NOP)
- An optional hook function pointer

| `patch_type` | Effect |
|---|---|
| `PATCH_HOOK` | Write a relative `jmp` (0xE9) to a hook function |
| `PATCH_CALL` | Write a relative `call` (0xE8) to a hook function |
| `HOOK_JE` / `HOOK_JNE` | Write a far conditional jump (0x0F 0x84/0x85) |
| `PATCH_BYTE` | Write a single byte value |
| `PATCH_INT` | Write a 4-byte integer value |
| `PATCH_FLOAT` | Write a 4-byte float value |
| `PATCH_NOP` | Fill `size` bytes with 0x90 |
| `PATCH_WRTE` | Parse a hex string and write it verbatim |
| `PATCH_FLOAT_PTR` / `PATCH_DOUBLE_PTR` | Repoint a data pointer to a float/double |

### Hook function pattern

Complex patches redirect game code to a `__declspec(naked)` hook function in the DLL using inline assembly (`__asm { ... }`). The hook:
1. Preserves needed registers and replicates overwritten instructions.
2. Runs new logic (reading hero/creature/spell state via vtable calls or game function pointers).
3. Jumps back to either the normal or an alternate game address.

Hook functions reference **hardcoded game addresses** (fork address, return address) defined at the top of each patch file.

## Shared Infrastructure

### `globals.cpp` / `globals.h`
- **Logging**: `initLog()` / `writeLog()` → `H5.log` in the working directory (controlled by `logs_enabled`).
- **Game function pointers**: typed as `int(__thiscall*)(int*)` (`GameFunction`) with hardcoded addresses for spell, hero, creature, buff notification, and other engine functions.
- **Shared constants**: `constf_*` / `consti_*` float/int constants shared across patches to avoid magic numbers.
- **Memory helpers**: `assignValueToAddress<T>()`, `assignHexStringToStruct()`.

### `utility_functions.cpp`
Low-level primitives used by `mainH5.cpp`'s apply loop:
- `JumpToFunction` / `ConditionalJumpToFunction` — write relative jmp/call bytes after `VirtualProtect`.
- NOP range fill.
- Byte/float/double pointer writes.
- `getXmlTree()` — reads XML from a `.pak` zip archive (dormant; `main()` does not currently load any pak).

### `types/` and `structs/`
Reverse-engineered definitions:
- `ARTIFACT_ID`, `SKILL_ID` enums — map game IDs to named constants for use in patches.
- `Skill_vtable`, `Hero_vtable`, `IHero` — struct/vtable layouts enabling typed vtable calls from inline asm.
- Helper functions: `count_equipped_artifact`, `get_skill_data`, `skill_mastery`, etc.

## Build & Deploy

- **IDE**: Visual Studio 2022, toolset v145, Win32 (x86), static CRT (`MultiThreaded`).
- **Output**: `H5_DLL.dll`.
- **Deploy**: `.sync-dll.ps1` renames the output to `um.dll` and copies it to `%H5_Folder%\bin\` (or the default Ubisoft ToE install path).
- **Dependencies**: pugixml (vendored), libzippp + zlib/bzip2 static libs (local, gitignored), MinHook (NuGet, currently unused in active code).

## Conventions

- **File naming**: `Mod*.cpp` for stat systems, `HeroSpec*.cpp` for specialization tweaks, `*Fix.cpp` for bugfixes.
- All balance values and target addresses are **hardcoded in source** — no runtime config is loaded.
- The `pugi::xml_document& doc` parameter in every `*_init` is wired for future XML config but is currently passed as an empty document.
- The `addresses.txt` file is the primary reference for game RVAs (artifacts, skills, hero specs, town buildings, unit stats, hero struct offsets).
