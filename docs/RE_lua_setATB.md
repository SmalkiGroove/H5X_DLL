# Reverse-Engineering Notes — Lua `setATB` & Combat Script Engine

Findings from the Ghidra MCP session of 2026-06-09 against the **H5X.exe** binary
(Ghidra project `h5x-ghidra-mcp/H5X.rep`). All addresses are virtual addresses (image base `0x00400000`).

---

## 1. H5X.exe PE layout

| Section | VA start | Virtual size | Raw offset | Raw size |
|---|---|---|---|---|
| `.text` | `0x00001000` | `0x00A09000` | `0x00000400` | `0x00A08200` |
| `.rdata` | `0x00A0A000` | `0x00115000` | `0x00A08600` | `0x00114200` |
| `.data` | `0x00B1F000` | `0x001CB000` | `0x00B1C800` | `0x00072200` |
| `.tls` | `0x00CEA000` | `0x00001000` | `0x00B8EA00` | `0x00000200` |
| `.rsrc` | `0x00CEB000` | `0x0004A400` | `0x00B8EC00` | `0x0004A400` |
| `mod ers` | `0x00D36000` | `0x00020000` | `0x00BD9000` | `0x00020000` |

File offset conversion for `.text`: `file_offset = VA - 0x400000 - 0x1000 + 0x400 = VA - 0x400C00`.

Note: `H5_Game.exe` (original ToE exe) has a different layout (`.text` VSize `0xB3F92A`, `.rdata` at `0xB41000`).
The Ghidra project addresses (e.g. vtable `0x00E18184` in `.rdata`) only match **H5X.exe**.

---

## 2. `lua_setATB` call chain

### 2.1 Overview

`setATB(unit, value)` in combat Lua scripts does **not** write ATB directly. Flow:

1. `lua_setATB` @ `0x0056CF60` parses the Lua args (unit name string, float value).
2. Resolves the unit: `arena->GetUnitByName(name)` via arena vtable `+0xC` (returns 0 → error
   `There is no unit on combat arena with name "%s"`).
3. Allocates a **0x14-byte command object** (`FUN_00878FD0(0x14)`) and constructs it with
   `FUN_00566800` (constructor of the setATB command, "CSetATBValue"):

   | Offset | Content |
   |---|---|
   | `+0x00` | vtable = `0x00E18184` |
   | `+0x04` | 0 |
   | `+0x08` | 0 |
   | `+0x0C` | unit pointer (refcounted via `FUN_004DD6E0`) |
   | `+0x10` | float value from Lua |

4. Posts the command to the arena via arena vtable `+0x4`. The command executes later in the
   combat command queue.

### 2.2 Command vtable `0x00E18184` (CSetATBValue)

| Slot | Address | Target | Meaning |
|---|---|---|---|
| `+0x00` | `0x00E18184` | `FUN_004E2C60` | release/clear |
| `+0x04` | `0x00E18188` | `FUN_00C98940` | stub `return 0` |
| `+0x08` | `0x00E1818C` | `FUN_00C26CE0` | scalar deleting destructor |
| `+0x0C` | `0x00E18190` | `0x004E3140` | (undefined in Ghidra) serialize? |
| `+0x10` | `0x00E18194` | `FUN_00C98940` | stub |
| `+0x14` | `0x00E18198` | `0x00C33EC0` | (undefined in Ghidra) |
| `+0x18` | `0x00E1819C` | `0x004018B0` | (undefined in Ghidra) |
| `+0x1C` | `0x00E181A0` | **`0x004E0970`** | **Execute** |

The next vtable (`PTR_FUN_00E181A8`) starts at `0x00E181A8` (different command class).

### 2.3 Execute method @ `0x004E0970` (not auto-defined as a function by Ghidra)

Manually disassembled from raw bytes:

```text
004E0970  8B 41 0C             mov  eax, [ecx+0xC]          ; unit from command
004E0973  85 C0                test eax, eax
004E0975  74 11                jz   004E0988                ; no unit -> return false
004E0977  8B 50 04             mov  edx, [eax+4]
004E097A  8B 52 04             mov  edx, [edx+4]
004E097D  F6 44 02 0B 80       test byte ptr [edx+eax+0xB], 0x80   ; unit-dead flag
004E0982  8D 44 02 04          lea  eax, [edx+eax+4]
004E0986  74 03                jz   004E098B
004E0988  32 C0                xor  al, al
004E098A  C3                   ret
004E098B  8B 41 0C             mov  eax, [ecx+0xC]          ; unit
004E098E  8B 49 10             mov  ecx, [ecx+0x10]         ; float value (bits)
004E0991  8B 10                mov  edx, [eax]              ; unit vtable
004E0993  51                   push ecx
004E0994  8B C8                mov  ecx, eax
004E0996  FF 92 8C 01 00 00    call dword ptr [edx+0x18C]   ; unit->SetATB(value)  <- overwrite
004E099C  B0 01                mov  al, 1
004E099E  C3                   ret
```

A disassembly comment documenting this was added in Ghidra at `0x004E0970`.

> Pitfall: the adjacent `FUN_004E09A0` is a **different** command's execute (reads two byte
> fields at `+0x10`/`+0x11`). An earlier patch attempt wrongly targeted `0x004E09E5` inside it.

---

## 3. Combat unit ATB interface (vtable offsets)

Verified through the `struct_CombatHero` vtable @ `0x00E8499C`:

| Vtable slot | Entry addr (hero vtable) | Thunk | Implementation | Meaning |
|---|---|---|---|---|
| `+0x184` | `0x00E84B20` | `0x00BC3250` | `0x008AEB40` | GetATBInfo (const/non-const pair with +0x188) |
| `+0x188` | `0x00E84B24` | `0x00BC3250` | `0x008AEB40` | **GetATBInfo** → returns ATB info object |
| `+0x18C` | `0x00E84B28` | `0x00BC2E90` | `0x00A6B690` | **SetATB(float)** |

- Thunks adjust `this` for virtual inheritance, e.g. `0x00BC3250`:
  `sub ecx,[ecx-4]; sub ecx,0x68; jmp impl`.
- Getter impl `0x008AEB40`: `mov eax, [ecx-0x70]; ret` → ATB info object pointer.
- `SetATB` impl `0x00A6B690` (`__thiscall(this, float)`, callee pops the arg):
  - Reads **current ATB** as `*(float*)(*(int*)(this-0x70) + 0x1C)` — i.e.
    `ATBInfo + 0x1C` is the current ATB float (consistent with `ImbueBalistaAtbFix.cpp`,
    which reads/writes hero ATB at `[obj+0x1C]`).
  - Stores the new value, sends a notification (vtable `+0xFC` call), and if the value crosses
    the ready threshold (`DAT_00E0A7E4`, presumably 1.0) triggers a "unit ready" virtual call.

Because both `GetATBInfo` and `SetATB` are called through the unit's own vtable, per-class
thunk adjustments (hero vs. creature) are handled automatically; `CATBInfo + 0x1C` is a
property of the shared ATB info class, not of the unit type.

---

## 4. The additive patch (`src/patches/LuaSetAtbAdditive.cpp`)

Goal: `setATB(unit, value)` **adds** `value` to current ATB instead of overwriting.

- `PATCH_HOOK` at fork `0x004E098B`, size `20` (covers `0x004E098B`–`0x004E099E`,
  the whole tail of the execute method; the hook never returns to the original code).
- The earlier `jz 0x004E098B` lands exactly on the hook's `jmp`; the failure path
  (`0x004E0988`: `xor al,al; ret`) is left intact.
- Hook logic:
  1. `unit = [cmd+0xC]`
  2. `info = unit->vtbl[+0x188](unit)` (GetATBInfo)
  3. `sum = [info+0x1C] + [cmd+0x10]` (x87 `fld`/`fadd`)
  4. `unit->vtbl[+0x18C](unit, sum)` (SetATB — callee pops the float arg)
  5. `al = 1; ret`
- Negative values now subtract naturally. Values crossing 1.0 are handled by `SetATB`'s
  own threshold logic.

Registered as `LuaSetAtbAdditive_init` in `mainH5.cpp` / `mainH5.h`, compiled via
`H5_DLL.vcxproj`.

---

## 5. Helper functions identified (engine internals)

| Address | Role |
|---|---|
| `0x00566800` | CSetATBValue constructor (vtable `0x00E18184`, unit @ `+0xC`, float @ `+0x10`) |
| `0x004DD6E0` | Store pointer with refcount increment |
| `0x00878FD0` | Engine allocator (`operator new`-like) |
| `0x008790B0` | Engine free/release |
| `0x00B0F690` | Lua: read args (string + value) |
| `0x00B0F650` | Lua: get script-context object (combat arena) |
| `0x00B0F610` | Lua: get error-output object |
| `0x00A47FD0` | printf-style message formatting |
| `0x00A2D6A0` / `0x00A2DBA0` | combat log/notification helpers used inside SetATB |

---

## 6. All `lua_*` functions in H5X.exe

268 functions, listed `name @ address`:

```text
lua_AddCreature @ 00568d90          lua_AddHeroCreatures @ 005c6140
lua_AddObjectCreatures @ 005ccec0   lua_AllowHeroHiringByRaceForAI @ 005e3d40
lua_AllowHeroHiringByRaceInTown @ 005e4270   lua_AllowHiringOfHeroForAI @ 005e4000
lua_AllowHiringOfHeroInTown @ 005e4530       lua_AllowOpenFogOfWarForAlly @ 005e51f0
lua_AllowPlayerTavernHero @ 005e4ac0         lua_AllowPlayerTavernRace @ 005e47f0
lua_BlockGame @ 005deeb0            lua_BlockTownGarrisonForAI @ 005e3aa0
lua_Break @ 00567660                lua_CalcAverageMonstersTier @ 005cfdc0
lua_CalcHeroMoveCost @ 005ceaf0     lua_CanMoveHero @ 005ce790
lua_CanShowToPlayer @ 005e8060      lua_ChangeHeroStat @ 005c3620
lua_ControlHeroCustomAbility @ 005d2560      lua_CreateArtifact @ 005c8730
lua_CreateCaravan @ 005e32c0        lua_CreateDwelling @ 005c9530
lua_CreateMonster @ 005c8aa0        lua_CreateStatic @ 005d1810
lua_CreateTreasure @ 005d1410       lua_DenyAIHeroFlee @ 005e22b0
lua_DenyAIHeroesFlee @ 005e2760     lua_DenyGarrisonCreaturesTakeAway @ 005d1fc0
lua_DeployReserveHero @ 005c27f0    lua_DestroyTownBuildingToLevel @ 005c8140
lua_DisableAutoEnterTown @ 005e6a00 lua_DisableCameraFollowHeroes @ 005df710
lua_DisableCameraFollowHeroesForPlayers @ 005df870
lua_DoNotGiveTurnToPlayerAIIfNoTowns @ 005e7750
lua_EnableAIHeroHiring @ 005da7a0   lua_EnableAutoFinish @ 00567210
lua_EnableCinematicCamera @ 0056e370         lua_EnableDynamicBattleMode @ 00566b60
lua_EnableHeroAI @ 005da500         lua_ExitGame @ 005e1480
lua_Finish @ 005673f0               lua_GenerateMonsters @ 005cf8c0
lua_GetAllNames @ 005d7f20          lua_GetArtifactSetItemsCount @ 005c57c0
lua_GetBuildingType @ 00568b60      lua_GetCreatureNumber @ 005686e0
lua_GetCreatureType @ 005684b0      lua_GetCurrentMoonWeek @ 005d2cc0
lua_GetCurrentPlayer @ 005d73d0     lua_GetDate @ 005d7170
lua_GetGameVar @ 005dea90           lua_GetGameVar2 @ 0056a3f0
lua_GetGuardsTier @ 005e0c30        lua_GetHeroArtifactsCount @ 005c4f70
lua_GetHeroCreatures @ 005c5ed0     lua_GetHeroCreaturesTypes @ 005ccb70
lua_GetHeroLevel @ 005c3190         lua_GetHeroName @ 00568290
lua_GetHeroSkillMastery @ 005c4140  lua_GetHeroStat @ 005c33a0
lua_GetHeroTown @ 005d0a90          lua_GetHost @ 00567000
lua_GetLastSavedCombatIndex @ 005e5c50       lua_GetMaxFloor @ 005e1570
lua_GetObjectArmySlotCreature @ 005d33f0     lua_GetObjectCreatures @ 005cc590
lua_GetObjectCreaturesTypes @ 005cc820       lua_GetObjectDwellingCreatures @ 005cd530
lua_GetObjectNamesByType @ 005e0660 lua_GetObjectOwner @ 005cb6d0
lua_GetObjectPosition @ 005caf40    lua_GetObjectiveProgress @ 005d8ea0
lua_GetObjectiveState @ 005d8710    lua_GetObjectsFromPath @ 005d0cf0
lua_GetObjectsInRegion @ 005cbf60   lua_GetPlayerHeroes @ 005c1540
lua_GetPlayerNecroEnergy @ 005d3900 lua_GetPlayerRace @ 005d2eb0
lua_GetPlayerResource @ 005c19a0    lua_GetPlayerSelectedCampaignBonusIndex @ 005d2830
lua_GetPlayerState @ 005c1390       lua_GetPlayerTeam @ 005d3670
lua_GetRageLevel @ 0056e710         lua_GetRagePoints @ 0056e4a0
lua_GetSavedCombatArmyCreatureInfo @ 005e6720
lua_GetSavedCombatArmyCreaturesCount @ 005e64d0
lua_GetSavedCombatArmyHero @ 005e6260        lua_GetSavedCombatArmyPlayer @ 005e6030
lua_GetSavedCombatResult @ 005e5e40 lua_GetStandState @ 005d0360
lua_GetStandStatesCount @ 005d0150  lua_GetTerrainSize @ 005e0f90
lua_GetTownBuildingLevel @ 005c7590 lua_GetTownBuildingLimitLevel @ 005c7b10
lua_GetTownBuildingMaxLevel @ 005c7850       lua_GetTownHero @ 005d0840
lua_GetTownRace @ 005e8c30          lua_GetTurnTimeLeft @ 005e8300
lua_GetUnitManaPoints @ 00569570    lua_GetUnitMaxManaPoints @ 00569790
lua_GetUnitPosition @ 00568060      lua_GetUnitSide @ 005677a0
lua_GetUnitType @ 005679f0          lua_GetUnits @ 00567c70
lua_GetWarMachineType @ 00568930    lua_GiveArtefact @ 005c5200
lua_GiveBorderguardKey @ 005c2540   lua_GiveHeroBattleBonus @ 005e89b0
lua_GiveHeroSkill @ 005c43a0        lua_GiveHeroWarMachine @ 005c6a50
lua_HasArtefact @ 005c4cd0          lua_HasBorderguardKey @ 005c22a0
lua_HasHeroSkill @ 005c3ee0         lua_HasHeroWarMachine @ 005c67f0
lua_IsAIPlayer @ 005d3140           lua_IsHeroAlive @ 005c2f80
lua_IsHeroInBoat @ 005e3580         lua_IsHeroInTown @ 005c9c50
lua_IsHeroLootable @ 005c5a70       lua_IsObjectEnabled @ 005cdbd0
lua_IsObjectExists @ 005c8520       lua_IsObjectInRegion @ 005cbcd0
lua_IsObjectVisible @ 005cc2a0      lua_IsObjectiveVisible @ 005d8120
lua_IsPlayerCurrent @ 005d75c0      lua_IsPlayerInGhostMode @ 005d7810
lua_IsPlayerLost @ 005e7db0         lua_IsPlayerWaitingForTurn @ 005d7a60
lua_IsRegionBlocked @ 005d97a0      lua_IsTeamCurrent @ 005d7cb0
lua_IsTilePassable @ 005e11a0       lua_KnowHeroSpell @ 005c4770
lua_LevelUpHero @ 005c3c00          lua_LockMinHeroSkillsAndAttributes @ 005e7010
lua_Loose @ 005d6ed0                lua_MakeHeroInteractWithObject @ 005d2220
lua_MakeHeroNecromancer @ 005e4d50  lua_MakeHeroReturnToTavernAfterDeath @ 005e4fc0
lua_MakeTownMovable @ 005e5770      lua_MarkObjectAsVisited @ 005ce0f0
lua_MessageBox @ 005db320           lua_MessageBox2 @ 0056e980
lua_MessageBoxForPlayers @ 005db910 lua_MoveCamera @ 005df150
lua_MoveCameraForPlayers @ 005df430 lua_MoveHero @ 005cef50
lua_MoveHeroRealTime @ 005cf310     lua_OpenCircleFog @ 005da1e0
lua_OpenPuzzleMap @ 005e29f0        lua_OpenRegionFog @ 005d9df0
lua_OverrideAdvMapDialogPos @ 005e3090
lua_OverrideObjectTooltipNameAndDescription @ 005e74e0
lua_Play2DSound @ 005dc970          lua_Play2DSound2 @ 0056eff0
lua_Play2DSoundForPlayers @ 005dcf40         lua_Play3DSound @ 005dcb90
lua_Play3DSound2 @ 0056f1e0         lua_Play3DSoundForPlayers @ 005dd170
lua_PlayObjectAnimation @ 005e1d90  lua_PlayVisualEffect @ 005e1710
lua_QuestionBox @ 005db5f0          lua_QuestionBoxForPlayers @ 005dbc30
lua_RazeBuilding @ 005ca830         lua_RazeTown @ 005ca3f0
lua_RegionToPoint @ 005d9540        lua_RemoveAllMonsters @ 005cfba0
lua_RemoveAllUnits @ 0056b400       lua_RemoveArtefact @ 005c54d0
lua_RemoveHeroCreatures @ 005c64a0  lua_RemoveHeroWarMachine @ 005c6d10
lua_RemoveObject @ 005cacb0         lua_RemoveObjectCreatures @ 005cd7b0
lua_ReplaceDwelling @ 005d1c50      lua_ResetHeroCombatScript @ 005c72e0
lua_SetAIHeroAttractor @ 005dfe10   lua_SetAIHeroFleeControl @ 005e2510
lua_SetAIPlayerAttractor @ 005e00e0 lua_SetAmbientLight @ 005dab00
lua_SetAmbientLightForPlayers @ 005dad90     lua_SetCombatAmbientLight @ 005e03d0
lua_SetControlMode @ 00566d70       lua_SetDisabledObjectMode @ 005e54b0
lua_SetGameVar @ 005decb0           lua_SetGameVar2 @ 0056a660
lua_SetHeroBiography @ 005d2a50     lua_SetHeroCombatScript @ 005c6fd0
lua_SetHeroLootable @ 005c5c80      lua_SetHeroRoleMode @ 005e7260
lua_SetHeroesExpCoef @ 005e2c70     lua_SetMonsterCourageAndMood @ 005c9240
lua_SetMonsterNames @ 005c8fd0      lua_SetMonsterSelectionType @ 005ce490
lua_SetObjectDwellingCreatures @ 005cd270    lua_SetObjectEnabled @ 005cde30
lua_SetObjectFlashlight @ 005db020  lua_SetObjectOwner @ 005cb910
lua_SetObjectPosition @ 005cb1b0    lua_SetObjectRotation @ 005cb440
lua_SetObjectiveProgress @ 005d9170 lua_SetObjectiveState @ 005d89e0
lua_SetObjectiveVisible @ 005d83f0  lua_SetPlayerHeroesCountNotForHire @ 005d11c0
lua_SetPlayerResource @ 005c1c30    lua_SetPlayerTeam @ 005e2030
lua_SetRegionAutoObjectEnable @ 005e6ca0     lua_SetRegionBlocked @ 005d9a50
lua_SetStandState @ 005d0570        lua_SetTownBuildingLimitLevel @ 005c7dd0
lua_SetTrigger @ 005dd530           lua_SetUnitManaPoints @ 005699b0
lua_SetWarfogBehaviour @ 005df9d0   lua_ShowFlyingSign @ 005dc450
lua_ShowFlyingSign2 @ 0056ebe0      lua_SiegeTown @ 005d4460
lua_SinkHero @ 005e37f0             lua_StartAdvMapDialog @ 005e2e80
lua_StartCombat @ 005d3b90          lua_StartCutScene @ 005de340
lua_StartCutSceneForPlayers @ 005de820       lua_StartDialogScene @ 005de0d0
lua_StartDialogSceneForPlayers @ 005de5a0    lua_StartPlayerStartResource @ 005c1fb0
lua_StopTrigger @ 005ddfa0          lua_StopVisualEffects @ 005e1bb0
lua_SummonCreature @ 00569180       lua_TakeAwayHeroExp @ 005e5a00
lua_TalkBoxForPlayers @ 005dbfa0    lua_TeachHeroSpell @ 005c49e0
lua_TransformTown @ 005c9ee0        lua_UnblockGame @ 005df000
lua_UnitCastAimedSpell @ 0056a080   lua_UnitCastAreaSpell @ 00569e60
lua_UnitCastGlobalSpell @ 00569c30  lua_UnreserveHero @ 005c2c90
lua_UpgradeTownBuilding @ 005e7a30  lua_WarpHeroExp @ 005e8720
lua_WarpToMap @ 005e85c0            lua_Win @ 005d69d0
lua_WinTeam @ 005d6c70              lua_addUnit @ 0056a9f0
lua_changeSubject @ 0056dbc0        lua_clearMessage @ 0056da90
lua_combatPlayEmotion @ 0056d5c0    lua_combatReadyPerson @ 0056cd60
lua_combatSetPause @ 0056d3c0       lua_combatStarted @ 0056cbd0
lua_commandDefend @ 0056b950        lua_commandDoSpecial @ 0056c4b0
lua_commandDoSpell @ 0056c830       lua_commandMove @ 0056bb30
lua_commandMoveAttack @ 0056bec0    lua_commandShot @ 0056c200
lua_displace @ 0056b680             lua_exist @ 0056ae00
lua_playAnimation @ 0056d170        lua_postEvent @ 0056a2f0
lua_random @ 005dfc30               lua_removeUnit @ 0056b1d0
lua_setATB @ 0056cf60               lua_shots @ 0056df10
lua_shotsNumber @ 0056e180          lua_showHighlighting @ 0056a890
lua_showMessage @ 0056d900          lua_subject @ 0056ddb0
lua_toggleTutorialMode @ 0056d7d0   lua_unitNames @ 0056afa0
```

---

## 7. Known vtable labels

| Address | Label | Notes |
|---|---|---|
| `0x00E8499C` | `struct_CombatHero` (user label) | Combat hero vtable; `+0x290` = HasSkill (used widely in patches as `call [edx+0x290]`), `+0x184`/`+0x188` = GetATBInfo, `+0x18C` = SetATB |
| `0x00E18184` | CSetATBValue command vtable | Execute at slot `+0x1C` → `0x004E0970` |
| `0x00E1D95C` | ICombatArenaCommon vtable (in `CCombatArenaScreen`) | The "arena vtable" used by `lua_setATB`; see section 8 |

Related RTTI strings found in `.rdata`:
`.?AVCSetATBValue@NWorld@@` @ `0x00F306E8`, `.?AVCATBInfo@NWorld@@` @ `0x00F45224`,
`.?AVCArenaATB@NWorld@@` @ `0x00F48A7C`, `"setATB"` @ `0x00E1E848`.

---

## 8. The combat arena vtable (`ICombatArenaCommon`)

`lua_setATB` obtains the arena via
`__RTDynamicCast(ctx, 0, SrcType, &ICombatArenaCommon_TD, 0)` with target type descriptor
`0x00F32224` = `.?AUICombatArenaCommon@NCombatArenaScript@@`. The interface is a **virtual
base**, so there is no single global vtable — each concrete arena class carries its own copy:

| Class | ICombatArenaCommon vtable | Subobject offset | Notes |
|---|---|---|---|
| `NCombatArenaScript::CCombatArenaImpl` (abstract base) | `0x00E1CAD4` | `+0x10` | 17 slots; slots 0–2 = `_purecall` (never instantiated directly) |
| `NUI::CCombatArenaScreen` (**the real combat arena**) | **`0x00E1D95C`** | `+0x57C` | 17 slots, adjustor thunks |
| `NUI::CCombatArenaFlybyScreen` (cutscene/flyby arena) | `0x00E32C14` area | tail subobject | second implementer |

At runtime, a combat script's `setATB` call goes through the `+0x57C` subobject of a
`CCombatArenaScreen`, so the dispatched vtable is `0x00E1D95C`.

### Interface slots (17 total, abstract vtable `0x00E1CAD4` ↔ screen vtable `0x00E1D95C`)

| Slot | Abstract (`CCombatArenaImpl`) | Screen (`CCombatArenaScreen`) | Meaning |
|---|---|---|---|
| `+0x00` | `_purecall` | `0x005528E0` → `0x0053E120` | (first interface method) |
| `+0x04` | `_purecall` | `0x005528F0` → `0x0053E110` | Post result/command (used by `lua_setATB`) |
| `+0x08` | `_purecall` | `0x00552900` | |
| `+0x0C` | `0x00565CA0` (forwarder) | `0x005527A0` = `add ecx,0x24; jmp 0x00565CA0` | **GetUnitByName** (returns combat unit or 0) |
| `+0x10`–`+0x1C` | `0x00565DC0`, `0x00565E20`, `0x00565CF0`, `0x00565CD0` | `0x005527B0`–`0x005527E0` thunks | unit/arena queries |
| `+0x20`–`+0x40` | `0x00565EC0` … `0x00566460` (several build command objects, like `FUN_00566090/300/580`) | `0x005527F0` … `0x00552910` thunks | script command factories |

Note: the adjacent 8-slot vtable `0x00E1D934` (subobject `+0x588`) is a **different**
interface of `CCombatArenaScreen` (slot `+0xC` → `0x0053ED10`, a per-unit flag setter) —
do not confuse it with the ICombatArenaCommon vtable.

### RTTI / layout facts used to derive this

- `CCombatArenaImpl` class hierarchy (CHD `0x00ECB4F4`): bases =
  self, `ICombatArenaCommon` (virtual, vbtable slot `+0x8`), `NCommonScript::IContext`
  (primary base *inside* the ICAC subobject, same vptr), `CObjectBase` (virtual, vbtable slot `+0x4`).
- `CCombatArenaImpl` COLs: subobject `+0x4` → vtable `0x00E1CB1C` (CObjectBase,
  recognizable by slot `+0x18` = `0x004018B0` refcount-release), `+0x10` → `0x00E1CAD4` (ICAC).
- `NUI::CCombatArenaScreen` (TD `0x00F32150`) embeds `CCombatArenaImpl` at `+0x174` and
  relocates virtual bases to the object tail: `+0x554` = CObjectBase (vtable `0x00E1D9D4`),
  `+0x57C` = ICombatArenaCommon (vtable `0x00E1D95C`), `+0x588` = unidentified 8-slot
  interface (vtable `0x00E1D934`).
- `NUI::CCombatArenaFlybyScreen` (TD `0x00F39368`) embeds `CCombatArenaImpl` at `+0x174`
  too; its tail vtables: `+0x248` → `0x00E32BFC`, `+0x23C` → `0x00E32C14`,
  `+0x230` → `0x00E32C5C`, `+0x220` → `0x00E32C7C`, `+0x0` → `0x00E32C9C`.
- IAT note: `0x00D11A02` (seen in abstract vtables) is `jmp [0x00E0A2A0]` =
  `MSVCR71.dll!_purecall`.
