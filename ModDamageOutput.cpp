#include "pch.h"

// EDIT DAMAGE OUTPUT MODIFIERS FROM ARTIFACTS
// (122) Sentinel's Shield : -10% ranged damage
// (133) Celestial Justicar Shield : -50% ranged damage

void MeleeDamageOutputFork();
void RangedDamageOutputFork();

int MeleeDamageOutput_fork = 0x00A5ACF9;
int MeleeDamageOutput_return = 0x00A5ACFF;
int RangedDamageOutput_fork = 0x00A5ACF9;
int RangedDamageOutput_return = 0x00A5ACFF;

void DamageOutput_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, MeleeDamageOutput_fork, 6, MeleeDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, RangedDamageOutput_fork, 6, RangedDamageOutputFork, 0, 0, 0 });
}

__declspec(naked) void MeleeDamageOutputFork() {
    __asm
    {
        
        jmp[MeleeDamageOutput_return]
    }
}

__declspec(naked) void RangedDamageOutputFork() {
    __asm
    {
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x7A
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_2
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_9]
        fstp dword ptr ss : [esp + 0x18]        

        RANGE_OUTPUT_2:
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x85
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_3
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_5]
        fstp dword ptr ss : [esp + 0x18]

        RANGE_OUTPUT_3:

        mov edx, dword ptr [edi + 0x4]
        mov eax, dword ptr [edx + 0x8]
        jmp[RangedDamageOutput_return]
    }
}
