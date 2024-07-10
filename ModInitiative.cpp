#include "pch.h"

// ADD INITIATIVE FROM ADDITIONAL ARTIFACTS
// (58) Moon Blade : +1 hero init
// (107) Ring of Celerity : +2 hero init

void CreatureInitiativeFork();
void HeroInitiativeFork();

int CreatureInitiative_fork = 0x00A348D8;
int CreatureInitiative_return = 0x00A348E7;
int HeroInitiative_fork = 0x00BC1F77;
int HeroInitiative_return = 0x00BC1F7D;

void Initiative_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, CreatureInitiative_fork, 5, CreatureInitiativeFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroInitiative_fork, 6, HeroInitiativeFork, 0, 0, 0 });
}

__declspec(naked) void CreatureInitiativeFork() {
    __asm
    {

    }
}

float moonblade_bonus = 1.0f;
float ring_of_celerity_bonus = 2.0f;
__declspec(naked) void HeroInitiativeFork() {
    __asm
    {
        mov eax, edx
        mov ecx, [esi - 0x6C]
        call dword ptr[edx + 0x74]
        mov ecx, eax
        push 0x3A
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_2
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr[moonblade_bonus]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_2:
        mov eax, edx
        mov ecx, [esi-0x6C]
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x6B
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_END
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [ring_of_celerity_bonus]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_END:
        mov ecx, dword ptr ss : [esp + 0xC]
        test ecx, ecx

        jmp[HeroInitiative_return]
    }
}
