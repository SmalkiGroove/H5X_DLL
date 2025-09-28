#include "pch.h"

// GIVE ARTIFACT 161 THE EFFECT OF SUPERNATURAL LUCK

void GoldenGooseFork();

int GoldenGoose_fork = 0x008A3F98;
int GoldenGoose_return = 0x008A3FB0;

void GoldenGoose_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, GoldenGoose_fork, 23, GoldenGooseFork, 0, 0, 0 });
}

__declspec(naked) void GoldenGooseFork() {
    __asm
    {
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        push 0xA1
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        jmp[GoldenGoose_return]
    }
}
