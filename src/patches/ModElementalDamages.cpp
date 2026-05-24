#include "pch.h"
#include "ElementsVirtuoso.h"

// ADD ADDITIONAL ELEMENTAL DAMAGE ARTIFACTS
// (5) Titan's Trident : from +50% to +20% to lightning spells
// (18) Evercold Icicle : from +50% to +20% to frost spells
// (32) Solar Ring : from +50% to +20% to fire spells
// (61) Emerald Slippers : from +50% to +20% to earth spells
// (192) Orb of Air Affinity : +33% to lightning spells
// (193) Orb of Earth Affinity : +33% to earth spells
// (194) Orb of Fire Affinity : +33% to fire spells
// (195) Orb of Water Affinity : +33% to frost spells
// (102) Altar of Elements Shard : +10% to all element spells
// Skill Obliteration (149) : +30% to all element spells
// Skill Elements Virtuoso (235) : +10% scaling when rotating elements

void ElemDamageFork();
void ElemDamageForkAlt();

int ElemDamage_fork = 0x00977086;
int ElemDamage_return = 0x00977091;
int ElemDamage_end = 0x0097712F;

void ElemDamage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ HOOK_JE, 0x00976EFE, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F1F, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F3B, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_WRTE, 0x00977062, 6, nullptr, 0, 0, 0, "D80D1C2FE100" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00977062, 6, nullptr, 0, 0, 0, "D80D1C2FE100" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00977013, 6, nullptr, 0, 0, 0, "D80D1C2FE100" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00977062, 6, nullptr, 0, 0, 0, "D80D1C2FE100" });
    assembly_patches.push_back({ PATCH_HOOK, ElemDamage_fork, 11, ElemDamageFork, 0, 0, 0 });
}

__declspec(naked) void ElemDamageFork() {
    __asm
    {
        mov ecx, dword ptr [ebp + 0x4]
        call[get_spell_element]
        cmp eax, 0x1
        je ELEMDAMAGE_LIGHTNING
        cmp eax, 0x2
		je ELEMDAMAGE_FIRE
		cmp eax, 0x3
		je ELEMDAMAGE_ICE
		cmp eax, 0x4
		je ELEMDAMAGE_EARTH

        jmp ELEMDAMAGE_RETURN

        ELEMDAMAGE_LIGHTNING:
        push 0xC0
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEMDAMAGE_ANY1
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
        call[notify_artifact_buff]
        jmp ELEMDAMAGE_ANY1

        ELEMDAMAGE_EARTH:
        push 0xC1
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEMDAMAGE_ANY1
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC1
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_artifact_buff]
        jmp ELEMDAMAGE_ANY1

        ELEMDAMAGE_FIRE:
        push 0xC2
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEMDAMAGE_ANY1
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC2
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_artifact_buff]
        jmp ELEMDAMAGE_ANY1

        ELEMDAMAGE_ICE:
        push 0xC3
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEMDAMAGE_ANY1
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr ds : 0x00E44660
        or ah, 0xC
        push 0xC3
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_artifact_buff]
        jmp ELEMDAMAGE_ANY1
    
        ELEMDAMAGE_ANY1:
        push 0x66
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je ELEMDAMAGE_ANY2
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr [constf_1_1]
        or ah, 0xC
        push 0x66
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_artifact_buff]

        ELEMDAMAGE_ANY2:
        push 0x95
        mov eax, dword ptr [esi]
        mov ecx, esi
        call dword ptr [eax + 0x290]
        test al, al
        jz ELEMDAMAGE_ANY3
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul dword ptr [constf_1_3]
        or ah, 0xC
        push 0x95
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_skill_buff]

        ELEMDAMAGE_ANY3:
        mov ecx, dword ptr [ebp + 0x4]
        call[get_spell_element]
        mov edx, eax
        mov ecx, esi
        call[elementVirtuoso_check]
        test eax, eax
        je ELEMDAMAGE_RETURN
        push eax
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_1_1]
        pop eax
        fild dword ptr [esp + 0x10]
        push 0x0
        fnstcw word ptr [esp + 0x18]
        push 0x0
        movzx eax, word ptr [esp + 0x1C]
        fmul st(0), st(1)
        or ah, 0xC
        push 0xEB
        mov dl, 0x1
        mov ecx, ebx
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        call[notify_skill_buff]

        ELEMDAMAGE_RETURN:
        cmp dword ptr [esp + 0x10], 0x0
        jle ELEMDAMAGE_END
        jmp[ElemDamage_return]

        ELEMDAMAGE_END:
        jmp[ElemDamage_end]
    }
}

__declspec(naked) void ElemDamageForkAlt() {
    __asm
    {
        cmp dword ptr[esp + 0x10], 0x0
        jle ELEMDAMAGE_END
        jmp[ElemDamage_return]
    ELEMDAMAGE_END:
        jmp[ElemDamage_end]
    }
}
