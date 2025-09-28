#include "pch.h"

// Fixes Imbue balista exhausting Hero ATB bug

void ImbueBalista_collect_atb();
void ImbueBalista_return_atb();

void ImbueBalista_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_WRTE, 0x00A59BEE, 1, nullptr, 0, 0, 0, "63" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00A59BFA, 1, nullptr, 0, 0, 0, "57" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00A59C0E, 1, nullptr, 0, 0, 0, "43" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00A59C28, 1, nullptr, 0, 0, 0, "29" });
    assembly_patches.push_back({ PATCH_WRTE, 0x00A59C37, 1, nullptr, 0, 0, 0, "50" });
    assembly_patches.push_back({ PATCH_CALL, 0x00A59C38, 8, ImbueBalista_collect_atb, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_CALL, 0x00A59C45, 8, ImbueBalista_return_atb, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_WRTE , 0x00A59C4D, 14, nullptr, 0, 0, 0, "5F5EC208008B44240C5F5EC20800" });
}

DWORD ImbueBalista_hero;
DWORD ImbueBalista_atb_value;
__declspec(naked) void ImbueBalista_collect_atb() {
    __asm
    {
        mov edx, dword ptr [esp + 0x1C]
        lea ecx, dword ptr [ecx + esi + 0x4]
        push ecx
        push esi
        push eax
        mov esi, ecx
        mov eax, dword ptr [esi - 0x118]
        mov ecx, dword ptr [eax + 0x8]
        lea ecx, dword ptr [ecx + esi - 0x118]
        sub ecx, dword ptr [ecx - 0x4]
        sub ecx, 0x68
        mov eax, dword ptr [ecx - 0x70]
        mov dword ptr [ImbueBalista_hero] , eax
        mov eax, dword ptr [eax + 0x1C]
        mov dword ptr [ImbueBalista_atb_value] , eax
        pop eax
        pop esi
        pop ecx
        ret
    }
}

__declspec(naked) void ImbueBalista_return_atb() {
    __asm
    {
        push eax
        push esi
        mov esi, dword ptr [ImbueBalista_hero]
        mov eax, dword ptr [ImbueBalista_atb_value]
        mov dword ptr [esi + 0x1C] , eax
        pop esi
        pop eax
        ret
    }
}