#include "pch.h"

// Spiteful ability increases attack by +3 per friendly stack killed (effect 209).
// Effects 299 and 314 share the same switch handler; branch on effect id.

void SpitefulAttackFork();

int SpitefulAttack_fork = 0x0098A5D5;
int SpitefulAttack_return = 0x0098A633;

void SpitefulTweak_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpitefulAttack_fork, 5, SpitefulAttackFork, 0, 0, 0 });
}

__declspec(naked) void SpitefulAttackFork() {
    __asm
    {
        cmp dword ptr [edi + 0x38], 0xD1
        jne spite_other
        mov eax, dword ptr [edi + 0x20]
        imul eax, 3
        add esi, eax
        jmp[SpitefulAttack_return]

    spite_other:
        add esi, dword ptr [edi + 0x20]
        jmp[SpitefulAttack_return]
    }
}

