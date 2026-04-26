#include "pch.h"

// CODEX RESTORES 15% ATB AFTER CASTING SPELL

void CodexOfTheSaintFork();

int CodexOfTheSaint_fork = 0x00BBF8D1;
int CodexOfTheSaint_return = 0x00BBF8D7;

void CodexOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CodexOfTheSaint_fork, 6, CodexOfTheSaintFork, 0, 0, 0 });
}

const float CodexOfTheSaint_atbRestore = 0.15f;
__declspec(naked) void CodexOfTheSaintFork() {
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
        je CODEX_OF_THE_SAINT_RETURN
        fld dword ptr ss: [esp + 0x1C]
		fadd dword ptr [CodexOfTheSaint_atbRestore]
		fstp dword ptr ss: [esp + 0x1C]

        CODEX_OF_THE_SAINT_RETURN:
        mov eax, dword ptr [esi - 0x7C]
        lea edi, dword ptr [esi - 0x7C]
        jmp[CodexOfTheSaint_return]
    }
}
