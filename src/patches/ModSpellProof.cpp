#include "pch.h"

// EDIT MAGIC PROOF FROM ARTIFACTS
// (13) Breastplate of the Forgotten Hero : 15% proof
// (174) Ancient Protection Charm : 10% proof
// (117) Genji's Vest : 20% proof

void SpellProofFork();

int SpellProof_fork = 0x0097AFC0;
int SpellProof_return = 0x0097AFC7;

void SpellProof_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpellProof_fork, 7, SpellProofFork, 0, 0, 0 });
}

__declspec(naked) void SpellProofFork() {
    __asm
    {
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0xD
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MAGICPROOF_2
        mov ecx, dword ptr ss: [esp + 0x4C]
        push 0x0
        push 0x0
        push 0xD
        xor dl, dl
        call[notify_artifact_buff]
        fld dword ptr ss: [esp + 0x48]
        fmul dword ptr [constf_0_85]
        fstp dword ptr ss: [esp + 0x48]

    MAGICPROOF_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0xAE
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MAGICPROOF_3
        mov ecx, dword ptr ss: [esp + 0x4C]
        push 0x0
        push 0x0
        push 0xAE
        xor dl, dl
        call[notify_artifact_buff]
        fld dword ptr ss: [esp + 0x48]
        fmul dword ptr [constf_0_9]
        fstp dword ptr ss: [esp + 0x48]

    MAGICPROOF_3:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0x75
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MAGICPROOF_END
        mov ecx, dword ptr ss: [esp + 0x4C]
        push 0x0
        push 0x0
        push 0x75
        xor dl, dl
        call[notify_artifact_buff]
        fld dword ptr ss: [esp + 0x48]
        fmul dword ptr [constf_0_8]
        fstp dword ptr ss: [esp + 0x48]

    MAGICPROOF_END:
        mov ecx, ebp
        call[get_spell_school]
        jmp[SpellProof_return]
    }
}
