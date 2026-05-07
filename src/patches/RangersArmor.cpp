#include "pch.h"

// RANGER'S ARMOR GRANTS +10 DEF VS LARGE CREATURES

void RangersArmorFork();

int RangersArmor_fork = 0x00A5AA83;
int RangersArmor_return = 0x00A5AA8A;

void RangersArmor_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, RangersArmor_fork, 6, RangersArmorFork, 0, 0, 0 });
}

__declspec(naked) void RangersArmorFork() {
    __asm
    {
        mov edx, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [edx + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
        test eax, eax
        je SKIP_RANGERS_ARMOR
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x95
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_RANGERS_ARMOR
        mov eax, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [eax + 0x44]
        cmp eax, 0x1
		jle SKIP_RANGERS_ARMOR
		add dword ptr ss: [esp + 0xC] , 0x10

        SKIP_RANGERS_ARMOR:
        mov edx, dword ptr [ebx]
        push 0x9F
        jmp[RangersArmor_return]
    }
}
