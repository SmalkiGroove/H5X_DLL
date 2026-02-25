#include "pch.h"

// INCREASE CREATRUES MAX DAMAGES BY 50%

void BloodyClawNecklaceFork();

int BloodyClawNecklace_fork = 0x004BC05C;
int BloodyClawNecklace_return = 0x004BC062;

void BloodyClawNecklace_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, BloodyClawNecklace_fork, 6, BloodyClawNecklaceFork, 0, 0, 0 });
}

__declspec(naked) void BloodyClawNecklaceFork() {
    __asm
    {
        mov edx, dword ptr [esi]
        mov ecx, esi
        push 0x6A
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
		test eax, eax
        je SKIP_BLOODYCLAWNECKLACE_BONUS
        mov ecx, edi
        shr ecx, 1
        add edi, ecx

        SKIP_BLOODYCLAWNECKLACE_BONUS:
        mov edx, dword ptr [esi]
        mov ecx, esi
        push 0x24

        jmp[BloodyClawNecklace_return]
    }
}
