#include "pch.h"
#include "types/Artifacts.h"

// ANKH OF THE SAINT (114) — luck floor 0 instead of -5 ([-5,5] -> [0,5])

void AnkhLuckPrologueFork();
void AnkhLuckClampFork();

static int* g_combat_luck_unit;

int AnkhLuckPrologue_fork = 0x00A6BD10;
int AnkhLuckPrologue_return = 0x00A6BD19;
int AnkhLuckClamp_fork = 0x00A6BD2D;

void AnkhOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, AnkhLuckPrologue_fork, 6, AnkhLuckPrologueFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AnkhLuckClamp_fork, 22, AnkhLuckClampFork, 0, 0, 0 });
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

static int __fastcall clamp_combat_luck_ankh(int raw_luck) {
    int luck = raw_luck;
    if (luck > 5)
        luck = 5;
    else if (luck < -5)
        luck = -5;
    if (luck < 0 && owner_has_ankh(g_combat_luck_unit))
        luck = 0;
    return luck;
}

// Save ICombatUnit* (inner, vtordisp -0xB0) before luck_raw fetch.
__declspec(naked) void AnkhLuckPrologueFork() {
    __asm
    {
        mov dword ptr [g_combat_luck_unit], ecx
        mov eax, dword ptr [ecx - 0xb0]
        mov edx, dword ptr [eax + 0x8]
        jmp[AnkhLuckPrologue_return]
    }
}

// Replace vanilla [-5,5] clamp after get_luck_raw (eax = raw luck).
__declspec(naked) void AnkhLuckClampFork() {
    __asm
    {
        mov ecx, eax
        call clamp_combat_luck_ankh
        ret
    }
}
