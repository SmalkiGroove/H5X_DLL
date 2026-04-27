#include "pch.h"

// EDIT SPELL PENETRATION FROM ARTIFACTS
// (119) Genji's Silksword : 20% spell penetration

void SpellPenetrationFork();

int SpellPenetration_fork = 0x0097D43B;
int SpellPenetration_return = 0x0097D464;
int SpellPenetration_end = 0x0097D47F;

void SpellPenetration_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpellPenetration_fork, 41, SpellPenetrationFork, 0, 0, 0 });
}

__declspec(naked) void SpellPenetrationFork() {
    __asm
    {
        mov edx, dword ptr [edi]
        mov ecx, edi
        call dword ptr [edx + 0x74]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x77
        call[count_equipped_artifact]
        test eax, eax
        je SPELL_PENETRATION_RETURN
        fld dword ptr ss: [esp + 0x10]
        fmul dword ptr [constf_0_8]
        fstp dword ptr ss: [esp + 0x10]
        
        SPELL_PENETRATION_RETURN:
        mov edx, dword ptr [edi]
        mov ecx, edi
        call dword ptr [edx + 0x74]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0x12
        call dword ptr [edx + 0x174]
		test eax, eax
        je SPELL_PENETRATION_END
        jmp[SpellPenetration_return]

        SPELL_PENETRATION_END:
        jmp[SpellPenetration_end]
    }
}
