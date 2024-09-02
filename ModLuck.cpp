#include "pch.h"

// ADD LUCK DEBUFFS FROM ADDITIONAL ARTIFACTS
// (63) Cursed Ring : change from -2 to -1 Luck
// (175) Spirit of Oppression : -1 Luck
// (99) Misfortune Dices : -2 Luck
// (164) Cloak of Darkness : -1 Luck
// also skill Dead Man's Curse -3 Luck

void LuckFork();

int Luck_fork = 0x00989E4E;
int Luck_return = 0x00989E70;

void Luck_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Luck_fork, 5, LuckFork, 0, 0, 0 });
}

__declspec(naked) void LuckFork() {
    __asm
    {
        neg eax
        lea esi, dword ptr [esi + eax*1]

        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0xAF
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        neg eax
        lea esi, dword ptr [esi + eax*1]

        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0x63
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        neg eax
        lea esi, dword ptr [esi + eax * 2]

        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0xA4
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        neg eax
        lea esi, dword ptr [esi + eax * 1]

        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0x67
        call dword ptr[edx + 0x290]
        test al, al
        jz LUCK_ERUINA
        sub esi, 0x3
        
     LUCK_ERUINA:
        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0x35
        call dword ptr [edx + 0x294]
        test al, al
        jz LUCK_END
        sub esi, 0x1

        LUCK_END:
        jmp[Luck_return]
    }
}
