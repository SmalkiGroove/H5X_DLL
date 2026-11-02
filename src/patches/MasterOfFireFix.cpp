#include "pch.h"

// Fix: Master of Fire always substracting half defense from the moment of impact instead of half of the current creature defense

void MasterOfFireFix();

void MasterOfFireFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x00989BB6, 1, nullptr, 9, 0, 0, 0 }); // disable bugged Master of Fire effect
    assembly_patches.push_back({ PATCH_HOOK, 0x008A25F4, 13, MasterOfFireFix, 0, 0, 0, 0 }); // new Master of Fire code
}

__declspec(naked) void MasterOfFireFix() {
    __asm
    {
        sub eax, 1
        and ebx, eax
        test ebx, ebx
        je DEFENSE_IS_ZERO
        mov ecx, esi
        mov edx, [esi]
        push 0xCA
        mov ecx, esi
        call dword ptr [edx]CombatUnit_vtable.get_active_buff
        test eax, eax
        je FIRE_EFFECT_NOT_PRESENT
        mov edx, [esi]
        push 0x55
        mov ecx, esi
        call dword ptr [edx]CombatUnit_vtable.has_ability
        test al, al
        jne UNIT_IS_ARMOURED
        // TO CHECK (possible crash, not reproduced): ebp is expected to be the own hero's ICombatHeroInterface, as set
        // at 0x008A2311 in func_GetUnitDefense (0x008A22F0). For town 9 creatures (creature data +0x98), the branch at
        // 0x008A24FF..0x008A2509 replaces it with the hero's CombatUnit subobject before this hook; slot 0 is then
        // get_unit_kind (returns 2) and the next mov edx,[eax] reads address 2. Only with buff 0xCA and without ability 0x55.
        test ebp, ebp
        je DEBUFF_DEFENSE
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax]CombatHeroInterface_vtable.get_hero
        mov edx, dword ptr [eax]
        push 0x54
        mov ecx, eax
        call dword ptr [edx]Hero_vtable.get_inventory
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        jne HERO_HAS_FROZEN_HEART

        DEBUFF_DEFENSE:
        mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax]CombatUnit_vtable.get_enemy_side
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]CombatArmy_vtable.get_combat_hero
        test eax, eax
        je debuff_defense_value
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]CombatHeroInterface_vtable.get_hero
        mov edx, dword ptr [eax]
		mov ecx, eax
        push 0xA3
		call dword ptr [edx]Hero_vtable.get_skill_mastery
        test eax, eax
        debuff_defense_value:
        push ebx
        fild dword ptr ss: [esp]
        je debuff_defense_noignite
        fmul dword ptr [constf_0_4]
        jmp debuff_defense_result
        debuff_defense_noignite:
        fmul dword ptr [constf_0_7]
        debuff_defense_result:
        fistp dword ptr ss: [esp]
        pop ebx

        DEFENSE_IS_ZERO:
        FIRE_EFFECT_NOT_PRESENT:
        UNIT_IS_ARMOURED:
        HERO_HAS_FROZEN_HEART:
        mov eax, ebx;
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 8
        ret
    }
}