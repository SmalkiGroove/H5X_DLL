#include "pch.h"

// EDIT ELEMENTAL RESISTANCE ARTIFACTS
// (9) Iceberg Shield : from +50% to +40% to fire resistance
// (20) Magnetic Ring : from +50% to +33% to lightning resistance
// (43) Dragon Flame Tongue : from +50% to +40% to ice resistance
// (62) Cloak of Sylanna : from +50% to +33% to earth resistance
// (84) Frozen Heart : from +25% to +33% to ice resistance
// (85) Phoenix Feather Cape : from +25% to +33% to fire resistance
// (124) Elrath Divine Shield : +40% to all element resistance

void ElemProofFork();
void FireProofAbilityFork();

int ElemProof_fork = 0x0097B0C5;
int ElemProof_return = 0x0097B0CB;
int ElemProof_end = 0x0097B23F;

int FireProofAbility_fork = 0x0097B172;
int FireProofAbility_return = 0x0097B23B;

float elemproof = 0.67f;

void ElemProof_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B0FF, 4, nullptr, 0, constf_0_6, 0, 0 }); // 9
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B237, 4, nullptr, 0, elemproof, 0, 0 }); // 20,62
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B1D3, 4, nullptr, 0, constf_0_6, 0, 0 }); // 43
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B204, 4, nullptr, 0, elemproof, 0, 0 }); // 84
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B130, 4, nullptr, 0, elemproof, 0, 0 }); // 85
    assembly_patches.push_back({ PATCH_HOOK, ElemProof_fork, 6, ElemProofFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, FireProofAbility_fork, 5, FireProofAbilityFork, 0, 0, 0 });
}

__declspec(naked) void ElemProofFork() {
    __asm
    {
        ja JUMPTABLE_DEFAULT_CASE
        test ebx, ebx
        je JUMPTABLE_DEFAULT_CASE

        mov eax, [ebx]
        mov ecx, ebx
        call dword ptr [eax + 0x74]
        push 0x7C
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je ELEM_RESIST_MAGIC_FILTER
        push 0x0
        push 0x0
        push 0x7C
        xor dl, dl
        mov ecx, esi
        call[notify_artifact_buff]
        fld dword ptr [esp + 0x48]
        fmul dword ptr [constf_0_6]
        fstp dword ptr [esp + 0x48]

        ELEM_RESIST_MAGIC_FILTER:
        test edi, edi
		je JUMPTABLE_ENTRYPOINT
        mov eax, [ebx]
        mov ecx, ebx
        push 0xE3
        call dword ptr [eax + 0x174]
        test eax, eax
        je JUMPTABLE_ENTRYPOINT
        xor edx, edx
        mov eax, edi
        sar eax, 8
        mov ecx, 0x4
        div ecx
        mov ecx, ebp
        call[get_spell_element]
        cmp edx, eax
        jne JUMPTABLE_ENTRYPOINT
        fld dword ptr [esp + 0x48]
        fmul dword ptr [constf_0_2]
        fstp dword ptr [esp + 0x48]

        JUMPTABLE_ENTRYPOINT:
        mov ecx, ebp
        call[get_spell_element]
        add eax, 0x0FFFFFFFF
        jmp[ElemProof_return]

        JUMPTABLE_DEFAULT_CASE:
        jmp[ElemProof_end]
    }
}


__declspec(naked) void FireProofAbilityFork() {
    __asm
    {
        fld dword ptr [esp + 0x48]
        fmul dword ptr [constf_0_5]
        jmp[FireProofAbility_return]
    }
}
