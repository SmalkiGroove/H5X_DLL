#include "pch.h"

// (114) Ankh of the Saint : the hero's creatures never have negative luck (luck floor 0 instead of -5).
// Patches the creature's func_GetUnitLuck (0x00A6ABF0, CombatUnit +0x1B4 get_luck_raw); func_CombatUnit_GetLuck (+0x1AC)
// then applies the vanilla [-5, 5] clamp.

void AnkhLuckBalanceFork();
void AnkhLuckModifiersFork();

// Exits of func_GetUnitLuck (esi = this, edi = luck from the creature and its hero; pop edi / pop esi / pop ebp / ret follow)
int AnkhLuckBalance_fork = 0x00A6AC90;      // week of balance (modifiers ignored): mov eax,edi
int AnkhLuckModifiers_fork_a = 0x00A6ACA7;  // + func_ComputeLuckModifiers (no unit): add eax,edi
int AnkhLuckModifiers_fork_b = 0x00A6ACC2;  // + func_ComputeLuckModifiers: add eax,edi

void AnkhOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, AnkhLuckBalance_fork, 6, AnkhLuckBalanceFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhLuckModifiers_fork_a, 6, AnkhLuckModifiersFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhLuckModifiers_fork_b, 6, AnkhLuckModifiersFork, 0, 0, 0 });
}

// luck_this = `this` of func_GetUnitLuck (vbptr at this-0xB0): the virtual base it reaches is the unit the function
// itself queries (vtable +0x70 / +0x80 / +0x08).
static int __fastcall ankh_luck_floor(char* luck_this, int luck) {
    if (luck >= 0) return luck;
    char* vbptr_holder = luck_this - 0xB0;
    int** vbtable = *(int***)vbptr_holder;
    ICombatUnit* unit = (ICombatUnit*)(vbptr_holder + (int)vbtable[2]);
    IHero* hero = owner_hero(unit);
    return hero && hero->has_artifact(ARTIFACT_ANKH_OF_THE_SAINT) ? 0 : luck;
}

__declspec(naked) void AnkhLuckBalanceFork() {
    __asm
    {
        mov edx, edi
        mov ecx, esi
        call ankh_luck_floor
        pop edi
        pop esi
        pop ebp
        ret
    }
}

// eax = luck modifiers
__declspec(naked) void AnkhLuckModifiersFork() {
    __asm
    {
        lea edx, dword ptr [eax + edi]
        mov ecx, esi
        call ankh_luck_floor
        pop edi
        pop esi
        pop ebp
        ret
    }
}
