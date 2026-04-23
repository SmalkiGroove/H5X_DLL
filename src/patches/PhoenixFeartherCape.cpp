#include "pch.h"

// PHOENIX FEATHER CAPE (85) IMMUNES TO FROST POWER

void PhoenixFeatherCapeFork();

int PhoenixFeatherCape_fork = 0x004E66C2;
int PhoenixFeatherCape_return = 0x004E66C7;
int PhoenixFeatherCape_end = 0x004E6730;

void PhoenixFeatherCape_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, PhoenixFeatherCape_fork, 5, PhoenixFeatherCapeFork, 0, 0, 0 });
}

__declspec(naked) void PhoenixFeatherCapeFork() {
    __asm
    {
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x55
        call[count_equipped_artifact]
        test eax, eax
        je PHOENIX_FEATHER_CAPE_RETURN
        jmp[PhoenixFeatherCape_end]
        
     PHOENIX_FEATHER_CAPE_RETURN:
        call[get_defaultstats]
        jmp[PhoenixFeatherCape_return]
    }
}
