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

// hero = the level-up code's hero object (ebp): IHero at +0x1C, max mana at +0x15C
static void __fastcall book_of_power_level_up(char* hero) {
    IHero* adv_hero = (IHero*)(hero + 0x1C);
    *(int*)(hero + 0x15C) = adv_hero->max_mana(adv_hero->knowledge());
    enqueue_unit_refresh(*(int*)hero);
}

__declspec(naked) void BookOfPowerLevelUpFork() {
    __asm
    {
        mov ecx, ebp
        call book_of_power_level_up
        jmp [BookOfPowerLevelUp_return]
    }
}
