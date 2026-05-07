#include "pch.h"

// ADD LUCK BUFFS FROM ADDITIONAL SOURCES
// skill Avenger +mastery Luck
// skill Precision +2 Luck

// ADD LUCK DEBUFFS FROM ADDITIONAL SOURCES
// (63) Cursed Ring : change from -2 to -1 Luck
// (175) Spirit of Oppression : -1 Luck
// (99) Misfortune Dices : -2 Luck
// (164) Cloak of Darkness : -1 Luck
// also skill Dead Man's Curse -3 Luck

void LuckBuffFork();
void LuckDebuffFork();
void LuckToggleFork();

int LuckBuff_fork = 0x00B4CA2B;
int LuckBuff_return = 0x00B4CA31;
int LuckDebuff_fork = 0x00989E4E;
int LuckDebuff_return = 0x00989E70;
int LuckToggle_fork = 0x008A3F0F;
int LuckToggle_return = 0x008A3F15;
int LuckToggle_end = 0x008A3F3B;

void Luck_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x00B4CA1E, 1, nullptr, 16, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, LuckBuff_fork, 6, LuckBuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LuckDebuff_fork, 5, LuckDebuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LuckToggle_fork, 6, LuckToggleFork, 0, 0, 0 });
}

__declspec(naked) void LuckBuffFork() {
    __asm
    {
        mov edx, dword ptr [esi]
        push 0x7C
        mov ecx, esi
        call dword ptr [edx + 0x174]
        test eax, eax
        je LUCK_RETURN
        add ebx, 0x2

        LUCK_RETURN:
        mov edi, dword ptr [esi + 0x228]
        jmp[LuckBuff_return]
    }
}

__declspec(naked) void LuckDebuffFork() {
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
        lea esi, dword ptr [esi + eax*2]

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
        lea esi, dword ptr [esi + eax*1]

        mov eax, [edi + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + edi + 0x4]
        lea ecx, [ecx + edi + 0x4]
        push 0x67
        call dword ptr [edx + 0x290]
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
        jmp[LuckDebuff_return]
    }
}

__declspec(naked) void LuckToggleFork() {
    __asm
    {
        mov edi, eax

        mov eax, dword ptr [esi - 0x144]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi - 0x144]
		lea ecx, dword ptr [ecx + esi - 0x144]
        call dword ptr [edx + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
        test eax, eax
        je DRAGONSBANE_RETURN
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xBB
        call[count_equipped_artifact]
        test eax, eax
        je DRAGONSBANE_RETURN
        mov edi, dword ptr [esi - 0x94]
        lea ecx, dword ptr [esi - 0x94]
        mov dword ptr ss: [esp + 0x8], ecx
        jmp[LuckToggle_end]
        
        DRAGONSBANE_RETURN:
        mov eax, edi
        mov edi, dword ptr [esi - 0x94]
        jmp[LuckToggle_return]
    }
}
