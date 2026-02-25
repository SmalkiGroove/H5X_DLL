#include "pch.h"

// ADD ABILITIES FROM OTHER SOURCES
// Perk STUN_TRAINING => Crushing Blow
// Perk ASSAULT_TRAINING => Violence
// Perk ARMOR_PROTECTION => Armored
// Perk BLOCK_CORRUPTION => Free Mind
// Perk UNLIMITED_RETALIATIONS => Unlimited Retaliations
// Perk FIERCE_RETALIATIONS => Fierce Retaliations
// (142) Crag Hack's Helmet => Battle Rage
// (160) Legendary Boots => Rider Charge
// (182) Ranger's Cape => Agility
// (162) ? => Child of Light
// (163) ? => Flame Wave

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
        // Crushing Blow
        cmp esi, 0x7B
        jne ABILITY_CHECK_VIOLENCE

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

        ABILITY_CHECK_VIOLENCE:
        // Violence
        cmp esi, 0x6F
        jne ABILITY_CHECK_ARMORED

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

        ABILITY_CHECK_ARMORED:
        // Armored
        cmp esi, 0x55
        jne ABILITY_CHECK_UNLIM_RETAL

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
        push 0xE4
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_UNLIM_RETAL:
		// Unlimited Retaliations
        cmp esi, 0x6
        jne ABILITY_CHECK_FIERCE_RETAL
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
        push 0xDA
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE
            
        ABILITY_CHECK_FIERCE_RETAL:
		// Fierce Retaliations
        cmp esi, 0x71
        jne ABILITY_CHECK_RIDER_CHARGE
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
        push 0xDB
        call dword ptr [eax + 0x290]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_RIDER_CHARGE:
        // Rider Charge
        cmp esi, 0x39
        jne ABILITY_CHECK_BATTLE_RAGE

        mov ecx, dword ptr [esp + 0x1C]
        cmp ecx, edi
        je ABILITIES_CHECK_NATIVE
        mov eax, dword ptr [ecx]
        call dword ptr [eax + 0x74]
        mov ecx, eax
        push 0xA0
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_BATTLE_RAGE:
        // Battle Rage
        cmp esi, 0xA2
        jne ABILITY_CHECK_AGILITY

        mov ecx, dword ptr [esp + 0x1C]
        cmp ecx, edi
        je ABILITIES_CHECK_NATIVE
        mov eax, dword ptr [ecx]
        call dword ptr [eax + 0x74]
        mov ecx, eax
        push 0x8E
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_AGILITY:
        // Agility
        cmp esi, 0x61
        jne ABILITY_CHECK_CHILD_OF_LIGHT

        mov ecx, dword ptr [esp + 0x1C]
        cmp ecx, edi
        je ABILITIES_CHECK_NATIVE
        mov eax, dword ptr [ecx]
        call dword ptr [eax + 0x74]
        mov ecx, eax
        push 0xB6
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_CHILD_OF_LIGHT:
        // Child of Light
        cmp esi, 0x8F
        jne ABILITY_CHECK_FLAME_WAVE

        mov ecx, dword ptr [esp + 0x1C]
        cmp ecx, edi
        je ABILITIES_CHECK_NATIVE
        mov eax, dword ptr [ecx]
        call dword ptr [eax + 0x74]
        mov ecx, eax
        push 0xA2
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_FLAME_WAVE:
        // Flame Wave
        cmp esi, 0xA6
        jne ABILITY_CHECK_HOLD_GROUND

        mov ecx, dword ptr [esp + 0x1C]
        cmp ecx, edi
        je ABILITIES_CHECK_NATIVE
        mov eax, dword ptr [ecx]
        call dword ptr [eax + 0x74]
        mov ecx, eax
        push 0xA3
        call[count_equipped_artifact]
        test eax, eax
        jne ABILITIES_RETURN_TRUE

        ABILITY_CHECK_HOLD_GROUND:
        // Hold Ground
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
        push 0xE5
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