#include "pch.h"

// INCREASE BALLISTA ATTACK BY 10

void RingOfDevastationFork();

int RingOfDevastation_fork = 0x004BC6A6;
int RingOfDevastation_return = 0x004BC6AC;

void RingOfDevastation_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, RingOfDevastation_fork, 6, RingOfDevastationFork, 0, 0, 0 });
}

__declspec(naked) void RingOfDevastationFork() {
    __asm
    {
        mov edx, dword ptr [esi]
        mov ecx, esi
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xA8
        call[count_equipped_artifact]
		test eax, eax
		je SKIP_RINGOFDEVASTATION_BONUS
        add edi, 0xA

        SKIP_RINGOFDEVASTATION_BONUS:
        mov eax, dword ptr [esi]
        mov ecx, esi
        push 0x8
        jmp[RingOfDevastation_return]
    }
}
