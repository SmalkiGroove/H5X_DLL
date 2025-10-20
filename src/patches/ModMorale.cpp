#include "pch.h"

// ADD MORALE BUFFS FROM ADDITIONAL SOURCES
// skill Motivation +1 Morale
// skill Last Stand +1 Morale

// ADD MORALE DEBUFFS FROM ADDITIONAL SOURCES
// (175) Spirit of Oppression : -1 Morale
// (176) Crimson Scythe : -1 Morale
// (125) Rod of Torment : -2 Morale
// skill Intimidating Voice -1 Morale
// skill Dead Man's Curse -3 Morale
// spec 80 (Sovereign) -1 Morale

void MoraleBuffFork();
void MoraleDebuffFork();

int MoraleBuff_fork = 0x00B4CAFB;
int MoraleBuff_return = 0x00B4CB01;
int MoraleDebuff_fork = 0x0098A175;
int MoraleDebuff_return = 0x0098A17A;

void Morale_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, MoraleBuff_fork, 6, MoraleBuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, MoraleDebuff_fork, 5, MoraleDebuffFork, 0, 0, 0 });
}

__declspec(naked) void MoraleBuffFork() {
    __asm
    {
        mov edx, dword ptr [esi]
        push 0x3D
        mov ecx, esi
        call dword ptr [edx + 0x174]
        test eax, eax
        je MORALE_NEXT_1
        add edi, 1

        MORALE_NEXT_1:
		mov edx, dword ptr [esi]
        push 0x76
        mov ecx, esi
        call dword ptr [edx + 0x174]
        test eax, eax
        je MORALE_NEXT_2
        add edi, 1

        MORALE_NEXT_2:

        MORALE_PARIAH:
        mov edx, dword ptr [esi]
        push 0x53
		mov ecx, esi
		jmp[MoraleBuff_return]
    }
}

__declspec(naked) void MoraleDebuffFork() {
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

        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0xA2
        call dword ptr [edx + 0x290]
        neg eax
        lea esi, dword ptr [esi + eax * 1]

        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0x67
        call dword ptr [edx + 0x290]
        test al, al
        jz MORALE_SOVEREIGN
        sub esi, 0x3

        MORALE_SOVEREIGN:
        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0x50
        call dword ptr [edx + 0x294]
        test al, al
        jz MORALE_END
        sub esi, 0x1

        MORALE_END:
        jmp[MoraleDebuff_return]
    }
}
