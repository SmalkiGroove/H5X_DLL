#include "pch.h"
#include "types/Artifacts.h"

// ANKH OF THE SAINT (114) — luck floor 0 instead of -5 for creature stacks.
// Patch func_GetUnitLuck (get_luck_raw @ 0x00A6ABF0, vtable +0x1B4).
// func_CombatUnit_GetLuck (+0x1AC) calls this first, then vanilla [-5,5] clamp — no extra hook needed.

void AnkhUnitLuckPrologueFork();
void AnkhUnitLuckExitMovFork();
void AnkhUnitLuckExitAddFork();

static int* g_combat_luck_unit;

int AnkhUnitLuckPrologue_fork = 0x00A6ABF0;
int AnkhUnitLuckPrologue_return = 0x00A6ABFA;
int AnkhUnitLuckExitMov_fork = 0x00A6AC90;
int AnkhUnitLuckExitAdd_fork_a = 0x00A6ACA7;
int AnkhUnitLuckExitAdd_fork_b = 0x00A6ACC2;

void AnkhOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, AnkhUnitLuckPrologue_fork, 6, AnkhUnitLuckPrologueFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhUnitLuckExitMov_fork, 6, AnkhUnitLuckExitMovFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhUnitLuckExitAdd_fork_a, 6, AnkhUnitLuckExitAddFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhUnitLuckExitAdd_fork_b, 6, AnkhUnitLuckExitAddFork, 0, 0, 0 });
}

static int __fastcall owner_has_ankh(int* inner) {
    int count = 0;
    __asm
    {
        push esi
        mov esi, inner
        test esi, esi
        jz ANKH_NOT_EQUIPPED
        mov eax, dword ptr [esi - 0xb0]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi - 0xb0]
        lea ecx, dword ptr [ecx + esi - 0xb0]
        call dword ptr [edx + 0xc]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xc]
        test eax, eax
        jz ANKH_NOT_EQUIPPED
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push ARTIFACT_ANKH_OF_THE_SAINT
        call[count_equipped_artifact]
        mov count, eax

    ANKH_NOT_EQUIPPED:
        pop esi
    }
    return count;
}

static int __fastcall apply_ankh_luck_floor(int luck) {
    if (luck < 0 && owner_has_ankh(g_combat_luck_unit))
        return 0;
    return luck;
}

// 6-byte hook spans push ebp/esi, mov esi,ecx, and the first 2 bytes of mov eax,[esi-0xB0].
__declspec(naked) void AnkhUnitLuckPrologueFork() {
    __asm
    {
        mov dword ptr [g_combat_luck_unit], ecx
        push ebp
        push esi
        mov esi, ecx
        mov eax, dword ptr [esi - 0xb0]
        jmp[AnkhUnitLuckPrologue_return]
    }
}

__declspec(naked) void AnkhUnitLuckExitMovFork() {
    __asm
    {
        mov eax, edi
        mov ecx, eax
        call apply_ankh_luck_floor
        pop edi
        pop esi
        pop ebp
        ret
    }
}

__declspec(naked) void AnkhUnitLuckExitAddFork() {
    __asm
    {
        add eax, edi
        mov ecx, eax
        call apply_ankh_luck_floor
        pop edi
        pop esi
        pop ebp
        ret
    }
}
