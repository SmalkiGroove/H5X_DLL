#include "pch.h"

// Fix: After level up hero max mana calculation does not take into account knowledge bonus from book of power.

void BookOfPowerLevelUp();

void BookOfPowerFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_NOP, 0x00B5A5CA, 5, nullptr, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, 0x00B5A5F2, 5, BookOfPowerLevelUp, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, 0x00B5A5D7, 5, BookOfPowerLevelUp, 0, 0, 0, 0 });
}

int BookOfPowerLevelUp_game_update = 0x0050C020;
int BookOfPowerLevelUp_game_return = 0x00B5A5F7;
__declspec(naked) void BookOfPowerLevelUp() {
    __asm
    {
        mov eax, dword ptr ss : [ebp + 0x1C]
        lea ecx, dword ptr ss : [ebp + 0x1C]
        call dword ptr ds : [eax + 0x1C]
        push eax
        mov eax, dword ptr ss : [ebp + 0x1C]
        lea ecx, dword ptr ss : [ebp + 0x1C]
        call dword ptr [eax + 0x130]
        mov [ebp + 0x15C], eax
        mov ecx, [ebp]
        call[BookOfPowerLevelUp_game_update]
        jmp[BookOfPowerLevelUp_game_return]
    }
}
