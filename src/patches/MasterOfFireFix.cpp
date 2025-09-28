#include "pch.h"

// Fix: Master of Fire always substracting half defense from the moment of impact instead of half of the current creature defense

void MasterOfFireFix();

void MasterOfFireFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x00989BB6, 1, nullptr, 9, 0, 0, 0 }); // disable bugged Master of Fire effect
    assembly_patches.push_back({ PATCH_HOOK, 0x008A25F4, 13, MasterOfFireFix, 0, 0, 0, 0 }); // new Master of Fire code
}

__declspec(naked) void MasterOfFireFix() {
    __asm
    {
        sub eax, 1
        and ebx, eax
        test ebx, ebx
        je DEFENSE_IS_ZERO
        mov ecx, esi
        mov edx, [esi]
        push 0xCA
        mov ecx, esi
        call dword ptr [edx + 0x28]
        test eax, eax
        je FIRE_EFFECT_NOT_PRESENT
        mov edx, [esi]
        push 0x55
        mov ecx, esi
        call dword ptr [edx + 0x28C]
        test al, al
        jne UNIT_IS_ARMOURED
        test ebp, ebp
        je HALVE_DEFENSE
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0x54
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        jne HERO_HAS_FROZEN_HEART

        HALVE_DEFENSE:
        shr ebx, 1

        DEFENSE_IS_ZERO:
        FIRE_EFFECT_NOT_PRESENT:
        UNIT_IS_ARMOURED:
        HERO_HAS_FROZEN_HEART:
        mov eax, ebx;
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 8
        ret
    }
}