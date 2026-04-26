#include "pch.h"

// COMBAT SKILL (13) AND CENTAUR CROSSBOW (153) INCREASES HERO'S ATTACKS DAMAGE

void HeroAttackDamageFork();
void HeroAttackDamageFork2();
void HeroAttackDamageFork3();

int HeroAttackDamage_fork = 0x00BC1D09;
int HeroAttackDamage_return = 0x00BC1D0F;
int HeroAttackDamage_fork2 = 0x00BC1D6D;
int HeroAttackDamage_return2 = 0x00BC1D73;
int HeroAttackDamage_fork3 = 0x00BC1D87;
int HeroAttackDamage_return3 = 0x00BC1D8E;

void HeroAttackDamage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork, 6, HeroAttackDamageFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork2, 6, HeroAttackDamageFork2, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork3, 7, HeroAttackDamageFork3, 0, 0, 0 });
}

__declspec(naked) void HeroAttackDamageFork() {
    __asm
    {
        mov dword ptr [varf], 0x0

        mov eax, dword ptr [esi]
        mov ecx, esi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0xD
        call dword ptr [edx + 0x174]
        test eax, eax
        je HERO_ATTACK_SKIP_COMBAT
        push eax
        fild dword ptr ss: [esp]
        fmul dword ptr [constf_0_2]
		fstp dword ptr ss: [esp]
        pop eax
        mov dword ptr [varf], eax

    HERO_ATTACK_SKIP_COMBAT:
        mov eax, dword ptr [esi]
        mov ecx, esi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x99
        call[count_equipped_artifact]
        test eax, eax
        je HERO_ATTACK_SKIP_CENTAUR_CROSSBOW
        mov eax, dword ptr [varf]
        push eax
        fld dword ptr ss: [esp]
        fadd dword ptr [constf_0_1]
        fstp dword ptr ss: [esp]
        pop eax
        mov dword ptr [varf], eax

    HERO_ATTACK_SKIP_CENTAUR_CROSSBOW:
        mov eax, dword ptr [esi - 0x9C]
        jmp[HeroAttackDamage_return]
    }
}

__declspec(naked) void HeroAttackDamageFork2() {
    __asm
    {
        fadd dword ptr [varf]
        fadd dword ptr [constf_1]
        mov dword ptr [varf], 0x0
        jmp[HeroAttackDamage_return2]
    }
}

__declspec(naked) void HeroAttackDamageFork3() {
    __asm
    {
        mov eax, dword ptr [varf]
        test eax, eax
		je HERO_ATTACK_DAMAGE_RETURN
        push eax
        fld dword ptr ss: [esp]
        fadd dword ptr [constf_1]
        pop eax
		fimul dword ptr ss: [esp + 0x10]
		fistp dword ptr ss: [esp + 0x10]

    HERO_ATTACK_DAMAGE_RETURN:
        mov eax, dword ptr ss: [esp + 0x10]
        cmp eax, 0x1
        jmp[HeroAttackDamage_return3]
    }
}
