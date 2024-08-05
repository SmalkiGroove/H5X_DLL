#include "pch.h"

// ADD SPEED FROM ADDITIONAL ARTIFACTS
// (57)  Boots of Swiftness : +2 Speed
// (157) Harpy Boots : +1 Speed
// (199) Unused moved from Ring of Caution : -1 Speed

void SpeedFork();

int Speed_fork = 0x004BC139;
int Speed_return = 0x004BC14F;

void Speed_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Speed_fork, 22, SpeedFork, 0, 0, 0 });
}

__declspec(naked) void SpeedFork() {
    __asm
    {
        mov edx, dword ptr [esi]
        push 0x39
        mov ecx, esi
        call dword ptr [edx + 74h]
        mov ecx, eax
        call[count_equipped_artifact]
        lea ebx, dword ptr [eax * 2]

        mov edx, dword ptr [esi]
        push 0x9D
        mov ecx, esi
        call dword ptr [edx + 74h]
        mov ecx, eax
        call[count_equipped_artifact]
        add ebx, eax

        mov     eax, [esi]
        push 0xC7
        // ... -1

        jmp[Speed_return]
    }
}
