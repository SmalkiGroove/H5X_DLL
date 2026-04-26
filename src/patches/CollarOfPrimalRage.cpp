#include "pch.h"

// ORCS GAIN 100 RAGE POINTS AT COMBAT START

void CollarOfPrimalRageFork();

int CollarOfPrimalRage_fork = 0x008AB32D;
int CollarOfPrimalRage_return = 0x008AB333;

void CollarOfPrimalRage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CollarOfPrimalRage_fork, 6, CollarOfPrimalRageFork, 0, 0, 0 });
}

__declspec(naked) void CollarOfPrimalRageFork() {
    __asm
    {
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x98
        call[count_equipped_artifact]
        test eax, eax
        je COLLAR_OF_PRIMAL_RAGE_RETURN
        mov eax, 0x64
        mov edx, dword ptr [esi + 0xA0]
        push 0x1
        lea ecx, dword ptr [esi + 0xA0]
        push 0x2710
        push eax
        call dword ptr [edx + 0x7C]

        COLLAR_OF_PRIMAL_RAGE_RETURN:
        mov ecx, dword ptr [ebp + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        jmp[CollarOfPrimalRage_return]
    }
}
