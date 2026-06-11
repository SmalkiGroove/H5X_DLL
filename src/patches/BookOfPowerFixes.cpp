#include "pch.h"

// Fix: After level up hero max mana calculation does not take into account knowledge bonus from book of power.

void BookOfPowerLevelUpFork();

int BookOfPowerLevelUp_fork1 = 0x00B5A5F2;
int BookOfPowerLevelUp_fork2 = 0x00B5A5D7;
int BookOfPowerLevelUp_return = 0x00B5A5F7;

void BookOfPowerFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_NOP, 0x00B5A5CA, 5, nullptr, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, BookOfPowerLevelUp_fork1, 5, BookOfPowerLevelUpFork, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, BookOfPowerLevelUp_fork2, 5, BookOfPowerLevelUpFork, 0, 0, 0, 0 });
}

__declspec(naked) void BookOfPowerLevelUpFork() {
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
        call[enqueue_unit_refresh]
        jmp[BookOfPowerLevelUp_return]
    }
}
