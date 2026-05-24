#include "pch.h"

// ADD BONUS SPELLPOWER FROM ADDITIONAL SOURCES
// Skill Blaze Power (231) : +3 for fire spells
// Skill Frost Power (232) : +3 for ice spells
// Skill Thunder Power (233) : +3 for lightning spells
// Skill Earth Power (234) : +3 for earth spells

void EffectiveSpellpowerFork();

int EffectiveSpellpower_fork = 0x00BC0508;
int EffectiveSpellpower_return = 0x00BC050E;

void EffectiveSpellpower_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, EffectiveSpellpower_fork, 6, EffectiveSpellpowerFork, 0, 0, 0 });
}

__declspec(naked) void EffectiveSpellpowerFork() {
    __asm
    {
        mov ecx, edi
        call[get_spell_element]
        cmp eax, 0x1
        je LABEL_THUNDER_POWER
        cmp eax, 0x2
        je LABEL_BLAZE_POWER
        cmp eax, 0x3
        je LABEL_FROST_POWER
        cmp eax, 0x4
        je LABEL_EARTH_POWER

        jmp LABEL_RETURN

        LABEL_BLAZE_POWER:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]
        push 0xE7
        call dword ptr [edx + 0x290]
        test al, al
        je LABEL_RETURN
        add ebx, 0x3
        push 0
        push 0
        push 0xE7
        mov dl, 0x1
        mov ecx, ebp
        mov dword ptr ss: [esp + 0x1C], ebx
        call[notify_skill_buff]
        
        LABEL_FROST_POWER:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]
        push 0xE8
        call dword ptr [edx + 0x290]
        test al, al
        je LABEL_RETURN
        add ebx, 0x3
        push 0
        push 0
        push 0xE8
        mov dl, 0x1
        mov ecx, ebp
        mov dword ptr ss: [esp + 0x1C], ebx
        call[notify_skill_buff]
        
        LABEL_THUNDER_POWER:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]
        push 0xE9
        call dword ptr [edx + 0x290]
        test al, al
        je LABEL_RETURN
        add ebx, 0x3
        push 0
        push 0
        push 0xE9
        mov dl, 0x1
        mov ecx, ebp
        mov dword ptr ss: [esp + 0x1C], ebx
        call[notify_skill_buff]
        
        LABEL_EARTH_POWER:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        mov edx, dword ptr [ecx + esi + 0x4]
        lea ecx, dword ptr [ecx + esi + 0x4]
        push 0xEA
        call dword ptr [edx + 0x290]
        test al, al
        je LABEL_RETURN
        add ebx, 0x3
        push 0
        push 0
        push 0xEA
        mov dl, 0x1
        mov ecx, ebp
        mov dword ptr ss: [esp + 0x1C], ebx
        call[notify_skill_buff]
        
        LABEL_RETURN:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        jmp[EffectiveSpellpower_return]
    }
}
