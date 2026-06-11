#pragma once

#ifndef TYPES_H
#define TYPES_H

#include "Skills.h"

// One typedef per calling convention + argument + return signature.

typedef int(__thiscall* ThiscallInt_IntPtr_Int)(int*, int);
typedef int(__thiscall* ThiscallInt_IntPtr)(int*);
typedef int(__thiscall* ThiscallInt_SkillId)(SKILL_ID);
typedef int*(__cdecl* CdeclIntPtr_Void)();
typedef int*(__thiscall* ThiscallIntPtr_IntPtr)(int*);
typedef int*(__thiscall* ThiscallIntPtr_Int)(int);
typedef int(__thiscall* ThiscallInt_Int_Int)(int, int);
typedef void(__fastcall* FastcallVoid_IntPtr_Char_3Int)(int*, char, int, int, int);
typedef void(__fastcall* FastcallVoid_Int)(int);
typedef float(__thiscall* ThiscallFloat_IntPtr)(int*);
typedef void(__thiscall* ThiscallVoid_IntPtr_Float)(int*, float);
typedef int(__thiscall* ThiscallInt_IntPtr_IntPtr_Int)(int*, int*, int);
typedef int(__thiscall* ThiscallInt_IntPtr_IntPtr)(int*, int*);
typedef int*(__thiscall* ThiscallIntPtr_IntPtr_IntPtr)(int*, int*);

// CombatUnit vtable slot types
using GetStat = ThiscallInt_IntPtr;
using GetSkillMastery = ThiscallInt_IntPtr_Int;
using HasHeroSpec = ThiscallInt_IntPtr_Int;
using GetUnitRef = ThiscallIntPtr_IntPtr;
using GetActiveBuffById = ThiscallInt_IntPtr_Int;
using HasCreatureAbility = ThiscallInt_IntPtr_Int;
using GetStatFloat = ThiscallFloat_IntPtr;
using GetATBInfo = ThiscallIntPtr_IntPtr;
using SetATBValue = ThiscallVoid_IntPtr_Float;
using GetCombatSpellpower = ThiscallInt_IntPtr_IntPtr_Int;

// AdvMap hero vtable slot types
using GetInventory = ThiscallIntPtr_IntPtr;
using GetMaxMana = ThiscallInt_IntPtr_Int;
using GetSpellpowerForSpell = ThiscallInt_IntPtr_IntPtr;
using GetNameWString = ThiscallIntPtr_IntPtr_IntPtr;
using GetPortraitTexture = ThiscallInt_IntPtr;
using GetAdvMapShared = ThiscallInt_IntPtr;

#endif
