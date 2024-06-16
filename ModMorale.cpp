#include "pch.h"

// ADD MORALE DEBUFFS FROM ADDITIONAL ARTIFACTS
// (175) Spirit of Oppression : -1 Morale
// (176) Crimson Scythe : -1 Morale
// (125) Rod of Torment : -2 Morale

void MoraleFork();

int Morale_fork = 0x0098A175;
int Morale_return = 0x0098A17A;

void Morale_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Morale_fork, 5, MoraleFork, 0, 0, 0 });
}

__declspec(naked) void MoraleFork() {
    __asm
    {
        neg eax
        lea esi, dword ptr [esi + eax * 2]

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
        lea esi, dword ptr [esi + eax * 1]

        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0x7D
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
        push 0xB0
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        neg eax
        lea esi, dword ptr [esi + eax * 1]

        jmp[Morale_return]
    }
}
