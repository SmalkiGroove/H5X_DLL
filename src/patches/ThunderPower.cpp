#include "pch.h"

// New skill that adds 10% damage as lightning

void ThunderPowerFork();

int ThunderPower_fork = 0x00A63554;
int ThunderPower_return = 0x00A63559;

void ThunderPower_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ThunderPower_fork, 5, ThunderPowerFork, 0, 0, 0 });
}

__declspec(naked) void ThunderPowerFork() {
    __asm
    {
        mov eax, [ebx]
        mov ecx, ebx
        call dword ptr [eax + 0xC]
        mov edx, [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
		test eax, eax
        je THUNDER_POWER_RETURN
        mov ecx, [eax + 0x4]
        mov edx, [ecx + 0x8]
        lea ecx, [edx + eax + 0x4]
        mov eax, [ecx]
        push 0x2D
        call dword ptr [eax + 0x290]
        test al, al
		je THUNDER_POWER_RETURN

        THUNDER_POWER_RETURN:
        push ebp
        push 0
        mov edx, ebx
        jmp[ThunderPower_return]
    }
}
