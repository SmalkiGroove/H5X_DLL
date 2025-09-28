#include "pch.h"

// EDIT MAGIC PROOF FROM ARTIFACTS
// (13) Breastplate of the Forgotten Hero : 10% proof
// (174) Ancient Protection Charm : 10% proof

void MagicProofFork();

int MagicProof_fork = 0x0097AFC0;
int MagicProof_return = 0x0097AFC7;

void MagicProof_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, MagicProof_fork, 7, MagicProofFork, 0, 0, 0 });
}

__declspec(naked) void MagicProofFork() {
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
        mov ecx, dword ptr ss : [esp + 0x4C]
        push 0x0
        push 0x0
        push 0xD
        xor dl, dl
        call[notify_artifact_buff]
        fld dword ptr ss : [esp + 0x48]
        fmul dword ptr [constf_0_9]
        fstp dword ptr ss : [esp + 0x48]

    MAGICPROOF_2:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0x74]
        push 0xAE
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        je MAGICPROOF_END
        mov ecx, dword ptr ss : [esp + 0x4C]
        push 0x0
        push 0x0
        push 0xAE
        xor dl, dl
        call[notify_artifact_buff]
        fld dword ptr ss : [esp + 0x48]
        fmul dword ptr [constf_0_9]
        fstp dword ptr ss : [esp + 0x48]

    MAGICPROOF_END:
        mov ecx, ebp
        call[get_spell_school]
        jmp[MagicProof_return]
    }
}
