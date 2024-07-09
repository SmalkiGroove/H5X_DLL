#include "pch.h"

// ADD ADDITIONAL ELEMENTAL DAMAGE ARTIFACTS
// (5) Titan's Trident : from +50% to +20% to lightning spells
// (18) Evercold Icicle : from +50% to +20% to frost spells
// (32) Phoenix Feather's Cape : from +50% to +20% to fire spells
// (61) Emerald Slippers : from +50% to +20% to earth spells
// (192) Orb of Air Affinity : +33% to lightning spells
// (193) Orb of Earth Affinity : +33% to earth spells
// (194) Orb of Fire Affinity : +33% to fire spells
// (195) Orb of Water Affinity : +33% to frost spells
// (102) Altar of Elements Shard : +10% to all element spells

void ElemDamageFork();
void ElemDamageForkAlt();

int ElemDamage_fork = 0x00977086;
int ElemDamage_return = 0x0097708B;
int ElemDamage_end = 0x0097712F;

void ElemDamage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ HOOK_JE, 0x00976EFE, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F1F, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F3B, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, ElemDamage_fork, 11, ElemDamageFork, 0, 0, 0 });
}

__declspec(naked) void ElemDamageFork() {
    __asm
    {
    ELEM_BONUS_AIR:
        mov ecx, dword ptr [ebp + 0x4]
        call[get_spell_element]
        cmp eax, 0x1
        jne ELEM_BONUS_EARTH
        push 0xC0
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEM_BONUS_EARTH
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC0
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[call_unknown_1]
    ELEM_BONUS_EARTH:
        mov ecx, dword ptr[ebp + 0x4]
        call[get_spell_element]
        cmp eax, 0x4
        jne ELEM_BONUS_FIRE
        push 0xC1
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEM_BONUS_FIRE
        fild dword ptr[esp + 0x10]
        push 0x0
        fnstcw word ptr[esp + 0x18]
        push 0x0
        movzx eax, word ptr[esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC1
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr[esp + 0x1C], eax
        fldcw word ptr[esp + 0x1C]
        fistp dword ptr[esp + 0x1C]
        fldcw word ptr[esp + 0x20]
        call[call_unknown_1]
    ELEM_BONUS_FIRE:
        mov ecx, dword ptr[ebp + 0x4]
        call[get_spell_element]
        cmp eax, 0x2
        jne ELEM_BONUS_WATER
        push 0xC2
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEM_BONUS_WATER
        fild dword ptr[esp + 0x10]
        push 0x0
        fnstcw word ptr[esp + 0x18]
        push 0x0
        movzx eax, word ptr[esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC2
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr[esp + 0x1C], eax
        fldcw word ptr[esp + 0x1C]
        fistp dword ptr[esp + 0x1C]
        fldcw word ptr[esp + 0x20]
        call[call_unknown_1]
    ELEM_BONUS_WATER:
        mov ecx, dword ptr[ebp + 0x4]
        call[get_spell_element]
        cmp eax, 0x3
        jne ELEM_BONUS_RETURN
        push 0xC3
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEM_BONUS_RETURN
        fild dword ptr[esp + 0x10]
        push 0x0
        fnstcw word ptr[esp + 0x18]
        push 0x0
        movzx eax, word ptr[esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC3
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr[esp + 0x1C], eax
        fldcw word ptr[esp + 0x1C]
        fistp dword ptr[esp + 0x1C]
        fldcw word ptr[esp + 0x20]
        call[call_unknown_1]
    ELEM_BONUS_RETURN:
        cmp dword ptr [esp + 0x10], 0x0
        jle ELEM_BONUS_END
        jmp[ElemDamage_return]
    ELEM_BONUS_END:
        jmp[ElemDamage_end]
    }
}

__declspec(naked) void ElemDamageForkAlt() {
    __asm
    {
        cmp dword ptr[esp + 0x10], 0x0
        jle ELEM_BONUS_END
        jmp[ElemDamage_return]
    ELEM_BONUS_END:
        jmp[ElemDamage_end]
    }
}
