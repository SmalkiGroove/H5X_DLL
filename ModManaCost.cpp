#include "pch.h"

// ADD MANA COST MODIFIERS FROM ADDITIONAL ARTIFACTS
// (47) Ring of the Magister : -6 mana
// (116) Genji's Hat : -1 mana
// (186) Moonlight Coat : -25% mana
// (101) Crown of the Frost Lord : -50% mana for frost spells
// (151) Blazing Spellbook : -33% mana for fire spells
// (140) Sylvan Amulet : -3 mana for nature spells
// (154) Deadwood Staff : -1 mana for dark spells

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
        sub esi, 0x6
        cmp esi, 0x1
        jge ring_of_magister_end
        mov esi, 0x1
        ring_of_magister_end:
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_1 :
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x74
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_2
        sub esi, 0x1
        cmp esi, 0x1
        jge genjis_hat_end
        mov esi, 0x1
        genjis_hat_end:
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
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x65
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_4
        mov ecx, edi
        call[get_spell_element]
        cmp eax, 0x3
        jne MANACOST_4
        mov eax, esi
        cdq
        sub eax, edx
        sar eax, 0x1
        mov esi, eax
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_4:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x97
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_5
        mov ecx, edi
        call[get_spell_element]
        cmp eax, 0x2
        jne MANACOST_5
        lea eax, dword ptr [esi + esi * 0x4]
        cdq
        sar eax, 0x3
        sar esi, 0x3
        add esi, eax
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_5:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x8C
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_6
        mov ecx, edi
        call[get_spell_school]
        cmp eax, 0x3
        jne MANACOST_6
        sub esi, 0x3
        cmp esi, 0x1
        jge sylvan_amulet_end
        mov esi, 0x1
        sylvan_amulet_end:
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_6:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x9A
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_END
        mov ecx, edi
        call[get_spell_school]
        cmp eax, 0x1
        jne MANACOST_END
        sub esi, 0x1
        cmp esi, 0x1
        jge deadwood_staff_end
        mov esi, 0x1
        deadwood_staff_end:
        mov dword ptr ss : [esp + 0x10] , esi

        MANACOST_END:
        jmp[ManaCost_return]
    }
}
