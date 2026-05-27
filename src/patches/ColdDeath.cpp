#include "pch.h"

// Cold Death doubles the freeze duration

void ColdDeathFreezeFork();

int ColdDeathFreeze_fork = 0x004E66CE;
int ColdDeathFreeze_return = 0x004E66D4;

void ColdDeathFreeze_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ColdDeathFreeze_fork, 6, ColdDeathFreezeFork, 0, 0, 0 });
}

__declspec(naked) void ColdDeathFreezeFork() {
    __asm
    {
        fmul dword ptr [ebp + 0x7A4]

        mov eax, dword ptr [edi]
		mov ecx, edi
		call dword ptr [eax + 0x10]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
        test eax, eax
        je COLD_DEATH_FREEZE_RETURN
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]
        mov edx, dword ptr [eax]
		mov ecx, eax
        push 0x6B
		call dword ptr [edx + 0x174]
        test eax, eax
        je COLD_DEATH_FREEZE_RETURN
        fmul dword ptr [constf_2]

        COLD_DEATH_FREEZE_RETURN:
        jmp[ColdDeathFreeze_return]
    }
}
