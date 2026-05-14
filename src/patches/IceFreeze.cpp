#include "pch.h"

// ICE SPELLS ARE ALWAYS AFFECTED BY MASTER OF ICE PERK

void IceFreezeFork1();
void IceFreezeFork2();

int IceFreeze_fork1 = 0x004E66B2;
int IceFreeze_return1 = 0x004E66C2;
int IceFreeze_fork2 = 0x009BB905;
int IceFreeze_return2 = 0x009BB911;

void IceFreeze_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, IceFreeze_fork1, 16, IceFreezeFork1, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, IceFreeze_fork2, 12, IceFreezeFork2, 0, 0, 0 });
}

__declspec(naked) void IceFreezeFork1() {
    __asm
    {
        jmp[IceFreeze_return1]
    }
}

__declspec(naked) void IceFreezeFork2() {
    __asm
    {
        mov eax, 0x1
        jmp[IceFreeze_return2]
    }
}
