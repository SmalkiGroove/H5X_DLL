#include "pch.h"

// Spear of the frost lord (170) ad 20% bonus ice damage

void SpearOfTheFrostLordFork();
void SpearOfTheFrostLordFork2();

int SpearOfTheFrostLord_fork = 0x00A5C016;
int SpearOfTheFrostLord_return = 0x00A5C02C;
int SpearOfTheFrostLord_fork2 = 0x00A5C090;
int SpearOfTheFrostLord_return2 = 0x00A5C096;

void SpearOfTheFrostLord_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpearOfTheFrostLord_fork, 22, SpearOfTheFrostLordFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, SpearOfTheFrostLord_fork2, 6, SpearOfTheFrostLordFork2, 0, 0, 0 });
}

__declspec(naked) void SpearOfTheFrostLordFork() {
    __asm
    {
        mov dword ptr [varf], 0x0

        mov ecx, dword ptr [eax + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        lea ecx, dword ptr [edx + eax + 0x4]
        mov eax, dword ptr [ecx]
        push 0x68
        call dword ptr [eax + 0x290]
        test al, al
        je SOTL_CHECK
        mov eax, dword ptr [varf]
        push eax
        fld dword ptr ss: [esp]
        fadd dword ptr [constf_0_1]
        fstp dword ptr ss: [esp]
        pop eax
        mov dword ptr [varf], eax

        SOTL_CHECK:
        mov eax, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xAA
        call[count_equipped_artifact]
        test eax, eax
        je SOTL_RETURN
        mov eax, dword ptr [varf]
        push eax
        fld dword ptr ss: [esp]
        fadd dword ptr [constf_0_2]
        fstp dword ptr ss: [esp]
        pop eax
        mov dword ptr [varf], eax

        SOTL_RETURN:
        mov eax, dword ptr [varf]
        test eax, eax
        jmp[SpearOfTheFrostLord_return]
    }
}

__declspec(naked) void SpearOfTheFrostLordFork2() {
    __asm
    {
        fmul dword ptr [varf]
        jmp[SpearOfTheFrostLord_return2]
    }
}
