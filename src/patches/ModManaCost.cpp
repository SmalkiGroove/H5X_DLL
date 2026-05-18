#include "pch.h"

// ADD MANA COST MODIFIERS FROM ADDITIONAL ARTIFACTS
// (47) Ring of the Magister : -4 mana
// (116) Genji's Hat : -1 mana
// (186) Moonlight Coat : -25% mana
// (101) Crown of the Frost Lord : -50% mana for frost spells
// (151) Blazing Spellbook : -33% mana for fire spells
// (140) Sylvan Amulet : -3 mana for nature spells
// (154) Deadwood Staff : -2 mana for dark spells
// Skill Blaze Power (231) : -10% mana for fire spells
// Skill Frost Power (232) : -10% mana for ice spells
// Skill Thunder Power (233) : -10% mana for lightning spells
// Skill Earth Power (234) : -10% mana for earth spells

void ManaCostFork();

int ManaCost_fork = 0x00A348D8;
int ManaCost_return = 0x00A348E7;

void ManaCost_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ManaCost_fork, 15, ManaCostFork, 0, 0, 0 });
}

__declspec(naked) void ManaCostFork() {
    __asm
    {
        je MANA_ALL_1
        sub esi, 0x4
        cmp esi, 0x1
        jge MANA_ALL_1
        mov esi, 0x1

        MANA_ALL_1 :
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x74
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANA_ALL_2
        sub esi, 0x1
        cmp esi, 0x1
        jge MANA_ALL_2
        mov esi, 0x1

        MANA_ALL_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0xBA
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANA_BY_ELEMENT
        mov eax, esi
        cdq
        sub eax, edx
        sar eax, 0x2
        sub esi, eax

        MANA_BY_ELEMENT:
        mov ecx, edi
        call[get_spell_element]
        cmp eax, 0x1
        je MANA_LIGHTNING_1
        cmp eax, 0x2
        je MANA_FIRE_1
        cmp eax, 0x3
        je MANA_ICE_1
        cmp eax, 0x4
        je MANA_EARTH_1

        jmp MANA_BY_SCHOOL

        MANA_ICE_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        push 0xE8
        call dword ptr [edx + 0x174]
        test eax, eax
        je MANA_ICE_2
        push esi
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_0_9]
        fistp dword ptr ss: [esp]
        pop esi

        MANA_ICE_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x65
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANA_BY_SCHOOL
        mov eax, esi
        cdq
        sub eax, edx
        sar eax, 0x1
        mov esi, eax
        jmp MANA_BY_SCHOOL

        MANA_FIRE_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        push 0xE7
        call dword ptr [edx + 0x174]
        test eax, eax
        je MANA_FIRE_2
        push esi
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_0_9]
        fistp dword ptr ss: [esp]
        pop esi

        MANA_FIRE_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x97
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANA_BY_SCHOOL
        lea eax, dword ptr [esi + esi * 0x4]
        cdq
        sar eax, 0x3
        sar esi, 0x3
        add esi, eax
        jmp MANA_BY_SCHOOL

        MANA_LIGHTNING_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        push 0xE9
        call dword ptr [edx + 0x174]
        test eax, eax
        je MANA_BY_SCHOOL
        push esi
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_0_9]
        fistp dword ptr ss: [esp]
        pop esi
        jmp MANA_BY_SCHOOL

        MANA_EARTH_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        push 0xEA
        call dword ptr [edx + 0x174]
        test eax, eax
        je MANA_BY_SCHOOL
        push esi
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_0_9]
        fistp dword ptr ss: [esp]
        pop esi

        MANA_BY_SCHOOL:
        mov ecx, edi
        call[get_spell_school]
        cmp eax, 0x1
        je MANA_DARK_1
        cmp eax, 0x3
        je MANA_NATURE_1

        jmp MANACOST_RETURN

        MANA_NATURE_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x8C
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_RETURN
        sub esi, 0x3
        cmp esi, 0x1
        jge MANACOST_RETURN
        mov esi, 0x1
        jmp MANACOST_RETURN

        MANA_DARK_1:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x9A
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MANACOST_RETURN
        sub esi, 0x1
        cmp esi, 0x1
        jge MANACOST_RETURN
        mov esi, 0x1

        MANACOST_RETURN:
        mov dword ptr ss: [esp + 0x10], esi
        jmp[ManaCost_return]
    }
}
