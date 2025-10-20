#include "pch.h"

// ADD ABILITIES FROM OTHER SOURCES
// Perk STUN_TRAINING => Bash
// Perk ASSAULT_TRAINING => Violence
// Perk ARMOR_PROTECTION => Armored
// Perk BLOCK_CORRUPTION => Free Mind

void CreatureAbilitiesFork();
void CreatureAbilityFreeMindFork();

int CreatureAbilities_fork = 0x008A361C;
int CreatureAbilities_return = 0x008A3622;

int CreatureAbilities_native = 0x008A3838;
int CreatureAbilities_enable = 0x008A3688;
int CreatureAbilities_disable = 0x008A36B9;

void CreatureAbilities_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureAbilities_fork, 6, CreatureAbilitiesFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, 0x008A36AC, 6, CreatureAbilityFreeMindFork, 0, 0, 0 });
}

__declspec(naked) void CreatureAbilitiesFork() {
    __asm
    {
        // Bash
        cmp esi, 0x1D
        jne ABILITY_CHECK_NEXT_1

        mov ecx, dword ptr [ebp - 0x144]
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + ebp - 0x144]
        lea edx, dword ptr [edx + ebp - 0x144]
        mov ecx, edx
        call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]

        cmp eax, edi
        je ABILITIES_CHECK_NATIVE
        mov ecx, dword ptr [eax + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        lea ecx, dword ptr [edx + eax + 0x4]
        mov eax, dword ptr [ecx]
        push 0x86
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_NEXT_1:
        // Violence
        cmp esi, 0x6F
        jne ABILITY_CHECK_NEXT_2

        mov ecx, dword ptr [ebp - 0x144]
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + ebp - 0x144]
        lea edx, dword ptr [edx + ebp - 0x144]
        mov ecx, edx
        call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]

        cmp eax, edi
        je ABILITIES_CHECK_NATIVE
        mov ecx, dword ptr [eax + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        lea ecx, dword ptr [edx + eax + 0x4]
        mov eax, dword ptr [ecx]
        push 0x87
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_NEXT_2:
        // Armored
        cmp esi, 0x55
        jne ABILITY_CHECK_NEXT_3

        mov ecx, dword ptr [ebp - 0x144]
        mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + ebp - 0x144]
        lea edx, dword ptr [edx + ebp - 0x144]
        mov ecx, edx
        call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0xC]

        cmp eax, edi
        je ABILITIES_CHECK_NATIVE
        mov ecx, dword ptr [eax + 0x4]
        mov edx, dword ptr [ecx + 0x8]
        lea ecx, dword ptr [edx + eax + 0x4]
        mov eax, dword ptr [ecx]
        push 0xD1
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_NEXT_3:
        // Hold ground
        cmp esi, 0xA5
        jmp[CreatureAbilities_return]

        ABILITIES_CHECK_NATIVE:
		jmp[CreatureAbilities_native]

        ABILITIES_RETURN_TRUE :
        jmp[CreatureAbilities_enable]

        ABILITIES_RETURN_FALSE :
        jmp[CreatureAbilities_disable]
    }
}

__declspec(naked) void CreatureAbilityFreeMindFork() {
    __asm
    {
        // check sandro's cloak
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITY_FREE_MIND_DISABLE

        mov ecx, dword ptr [ebp - 0x144]
		mov edx, dword ptr [ecx + 0x8]
        mov eax, dword ptr [edx + ebp - 0x144]
        lea edx, dword ptr [edx + ebp - 0x144]
        mov ecx, edx
		call dword ptr [eax + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
		call dword ptr [edx + 0xC]

        cmp eax, edi
        je ABILITY_FREE_MIND_END
        mov ecx, dword ptr [eax + 0x4]
		mov edx, dword ptr [ecx + 0x8]
        lea ecx, dword ptr [edx + eax + 0x4]
        mov eax, dword ptr [ecx]
        push 0xD0
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITY_FREE_MIND_ENABLE

        ABILITY_FREE_MIND_END:
        jmp[CreatureAbilities_native]

        ABILITY_FREE_MIND_ENABLE:
        jmp[CreatureAbilities_enable]

        ABILITY_FREE_MIND_DISABLE:
		jmp[CreatureAbilities_disable]
    }
}