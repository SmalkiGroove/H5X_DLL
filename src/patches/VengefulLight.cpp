#include "pch.h"

// New skill that has the effect of Vessel of Elrath spec
// and move Eternal Light to Master of Abjuration

void VengefulLightFork();

int VengefulLight_fork = 0x00BBEDA5;
int VengefulLight_return = 0x00BBEDB3;
int VengefulLight_end = 0x00BBEDC1;

void VengefulLight_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x004E4629, 1, nullptr, 0, 0, 0, "32" });
    assembly_patches.push_back({ PATCH_HOOK, VengefulLight_fork, 14, VengefulLightFork, 0, 0, 0 });
}

__declspec(naked) void VengefulLightFork() {
    __asm
    {
        mov ecx, edi
        push 0x52
        call dword ptr [eax + 0x294]
        test al, al
        jnz VENGEFUL_LIGHT_RETURN

        mov eax, dword ptr [edi]
        mov ecx, edi
        push 0xA5
        call dword ptr [eax + 0x290]
        test al, al
        jnz VENGEFUL_LIGHT_RETURN

        jmp[VengefulLight_end]

    VENGEFUL_LIGHT_RETURN:
        jmp[VengefulLight_return]
    }
}
