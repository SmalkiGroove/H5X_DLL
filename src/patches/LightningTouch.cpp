#include "pch.h"

// New skill that adds 10% damage as lightning

void LightningTouchFork();

int LightningTouch_fork = 0x00A63554;
int LightningTouch_return = 0x00A63559;

void LightningTouch_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, LightningTouch_fork, 5, LightningTouchFork, 0, 0, 0 });
}

__declspec(naked) void LightningTouchFork() {
    __asm
    {
        mov eax, [esi]
        mov ecx, esi
        call dword ptr [eax + 0xC]
        mov edx, [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
		test eax, eax
        je LIGHTNING_TOUCH_RETURN
        mov ecx, [eax + 0x4]
        mov edx, [ecx + 0x8]
        lea ecx, [edx + eax + 0x4]
        mov eax, [ecx]
        push 0x2D
        call dword ptr [eax + 0x290]
        test al, al
		je LIGHTNING_TOUCH_RETURN
        mov eax, dword ptr ss : [esp + 0x4]
        push ebp
        fild dword ptr ss : [esp]
        fmul dword ptr [constf_0_1]
        mov dword ptr ss : [esp], 0
        push 1
        push eax
        push 0
        push esi
        push ecx
        fstp dword ptr ss : [esp]
        mov ecx, 0x4
        xor edx, edx
        call[get_mitigated_damage]
        test eax, eax
        // TODO visual effect
        add ebp, eax

        LIGHTNING_TOUCH_RETURN:
        push ebp
        push 0
        mov edx, ebx
        jmp[LightningTouch_return]
    }
}
