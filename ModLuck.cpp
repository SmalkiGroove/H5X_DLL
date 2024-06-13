#include "pch.h"

// ADD LUCK DEBUFFS FROM ADDITIONAL ARTIFACTS
// (63) Cursed Ring : change from -2 to -1 Luck
// (175) Spirit of Oppression : -1 Luck
// (99) Cursed Dices : -2 Luck
// (164) Cloak of Darkness : -1 Luck

void LuckFork();

int Luck_fork = 0x00989E4E;
int Luck_return = 0x00989E53;

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

        jmp[Luck_return]
    }
}
