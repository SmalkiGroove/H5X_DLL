#include "pch.h"

// EDIT DAMAGE OUTPUT MODIFIERS FROM ARTIFACTS
// (122) Sentinel's Shield : -10% ranged damage
// (133) Celestial Justicar Shield : -50% ranged damage
// (198) Shield of Dwarven Kings : -30% ranged damage
// Skill Courage reduce all damages per Morale point

void MeleeDamageOutputFork();
void RangedDamageOutputFork();
void AllDamageOutputFork();

int MeleeDamageOutput_fork = 0x00A5ACF9;
int MeleeDamageOutput_return = 0x00A5ACFF;
int RangedDamageOutput_fork = 0x00A5ACF9;
int RangedDamageOutput_return = 0x00A5ACFF;
int AllDamageOutput_fork = 0x00A5AD6E;
int AllDamageOutput_return = 0x00A5AD75;

void DamageOutput_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, MeleeDamageOutput_fork, 6, MeleeDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, RangedDamageOutput_fork, 6, RangedDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AllDamageOutput_fork, 7, AllDamageOutputFork, 0, 0, 0 });
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
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xC6
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_END
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_7]
        fstp dword ptr ss : [esp + 0x18]

    RANGE_OUTPUT_END:
        mov edx, dword ptr [edi + 0x4]
        mov eax, dword ptr [edx + 0x8]
        jmp[RangedDamageOutput_return]
    }
}

__declspec(naked) void AllDamageOutputFork() {
    __asm
    {
        test edi, edi
        jz COURAGE_END
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0x4
        call dword ptr [edx + 0x174]
        test eax, eax
        jz COURAGE_END
        mov ecx, dword ptr ss : [esp + 0x20]
        mov dword ptr ss : [esp + 0x20], eax
        fild dword ptr ss : [esp + 0x20]
        fmul dword ptr [constf_percent]
        mov dword ptr ss : [esp + 0x20], ecx
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax + 0x1B0]
        mov ecx, dword ptr ss : [esp + 0x20]
        mov dword ptr ss : [esp + 0x20], eax
        fimul dword ptr ss : [esp + 0x20]
        fsubr dword ptr [constf_1]
        fmul dword ptr ss : [esp + 0x18]
        fstp dword ptr ss : [esp + 0x18]
        mov dword ptr ss : [esp + 0x20], ecx

        COURAGE_END:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0xC]

        jmp[AllDamageOutput_return]
    }
}
