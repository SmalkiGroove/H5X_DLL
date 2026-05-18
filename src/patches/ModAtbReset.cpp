#include "pch.h"

// RESTORE ATB AFTER CASTING SPELL
// (126) Codex of the Saint : 15%
// Skill Elemental Focus : 10% (only for elemental spells)

void AtbResetFork();

int AtbReset_fork = 0x00BBF8D1;
int AtbReset_return = 0x00BBF8D7;

void AtbReset_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, AtbReset_fork, 6, AtbResetFork, 0, 0, 0 });
}

__declspec(naked) void AtbResetFork() {
    __asm
    {
        lea ecx, dword ptr [esi - 0x7C]
        mov eax, dword ptr [ecx]
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x7E
        call[count_equipped_artifact]
        test eax, eax
        je ATB_RESET_ELEMENTAL_FOCUS
        fld dword ptr ss: [esp + 0x1C]
		fadd dword ptr [constf_0_15]
		fstp dword ptr ss: [esp + 0x1C]
        
        ATB_RESET_ELEMENTAL_FOCUS:
        mov ecx, dword ptr [ebx + 0x4]
        call[get_spell_element]
        test eax, eax
		je ATB_RESET_RETURN

        lea ecx, dword ptr [esi - 0x7C]
        mov eax, dword ptr [ecx]
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0xE6
        call dword ptr [edx + 0x174]
        test eax, eax
        je ATB_RESET_RETURN
        fld dword ptr ss: [esp + 0x1C]
        fadd dword ptr [constf_0_1]
        fstp dword ptr ss: [esp + 0x1C]


        ATB_RESET_RETURN:
        mov eax, dword ptr [esi - 0x7C]
        lea edi, dword ptr [esi - 0x7C]
        jmp[AtbReset_return]
    }
}
