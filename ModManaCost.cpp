#include "pch.h"

// ADD MANA COST MODIFIERS FROM ADDITIONAL ARTIFACTS
// (47) Ring of the Magister : -8 mana
// (112) Fortune Band of the Saint : -1 Mana
// (186) Moonlight Coat : -25% mana
// (101) Crown of the Frost Lord : -50% mana for frost spells

void ManaCostFork();

int ManaCost_fork = 0x00A348D8;
int ManaCost_return = 0x00A348E7;

void ManaCost_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ManaCost_fork, 15, ManaCostFork, 0, 0, 0 });
}

__declspec(naked) void ManaCostFork() {
    __asm
    {
        je MANACOST_1
        sub esi, 0x8
        cmp esi, 0x1
        jge ring_of_magister_end
        mov esi, 0x1
        ring_of_magister_end:
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_1 :
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x70
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_2
        sub esi, 0x1
        cmp esi, 0x1
        jge fortune_band_of_the_saint_end
        mov esi, 0x1
        fortune_band_of_the_saint_end:
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0xBA
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_3
        mov eax, esi
        cdq
        sub eax, edx
        sar eax, 0x2
        sub esi, eax
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_3:
        mov edx, dword ptr[ebx]
        mov ecx, ebx
        call dword ptr[edx + 0x74]
        push 0x65
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_END
        mov ecx, edi
        call[get_spell_element]
        cmp eax, 0x3
        jne MANACOST_END
        mov eax, esi
        cdq
        sub eax, edx
        sar eax, 0x1
        mov esi, eax
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_END:
        jmp[ManaCost_return]
    }
}
