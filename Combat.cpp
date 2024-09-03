#include "pch.h"

// COMBAT SKILL INCREASES HERO ATTACKS DAMAGE 

void CombatSkillFork();

int CombatSkill_fork = 0x00BC1D09;
int CombatSkill_return = 0x00BC1D0F;

void CombatSkill_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CombatSkill_fork, 6, CombatSkillFork, 0, 0, 0 });
}

__declspec(naked) void CombatSkillFork() {
    __asm
    {
        mov eax, dword ptr [esi]
        mov ecx, esi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0xD
        call dword ptr [edx + 0x174]
        mov ecx, dword ptr ss : [esp + 0x1C]
        mov dword ptr ss : [esp + 0x1C], eax
        fild dword ptr ss : [esp + 0x1C]
        fmul dword ptr [constf_0_2]
        fadd dword ptr [constf_1]
        fimul dword ptr ss : [esp + 0x10]
        fistp dword ptr ss : [esp + 0x10]
        mov dword ptr ss : [esp + 0x1C] , ecx

        mov eax, dword ptr [esi - 0x9C]
        jmp[CombatSkill_return]
    }
}
