#include "pch.h"

// ARCANE BRILLANCE REDUCE MANA COST BY 25%

void ArcaneBrillanceFork();

int ArcaneBrillance_fork = 0x009783AB;
int ArcaneBrillance_return = 0x009783B1;

void ArcaneBrillance_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ArcaneBrillance_fork, 6, ArcaneBrillanceFork, 0, 0, 0 });
}

__declspec(naked) void ArcaneBrillanceFork() {
    __asm
    {
        lea ebx, [eax + 2*eax]
		sar ebx, 2
        cmp ebx, edi
        jmp[ArcaneBrillance_return]
    }
}
