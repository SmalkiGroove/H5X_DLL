#include "pch.h"

// COMBAT SKILL (13) AND CENTAUR CROSSBOW (153) INCREASES HERO'S ATTACKS DAMAGE
// +20% per Combat mastery level, +10% with the Centaur Crossbow.

void HeroAttackDamageFork();
void HeroAttackDamageFork2();
void HeroAttackDamageFork3();

int HeroAttackDamage_fork = 0x00BC1D09;
int HeroAttackDamage_return = 0x00BC1D0F;
int HeroAttackDamage_fork2 = 0x00BC1D6D;
int HeroAttackDamage_return2 = 0x00BC1D73;
int HeroAttackDamage_fork3 = 0x00BC1D87;
int HeroAttackDamage_return3 = 0x00BC1D8E;

static float hero_attack_bonus = 0.0f;   // set by the first hook, used and reset by the next two

void HeroAttackDamage_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork, 6, HeroAttackDamageFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork2, 6, HeroAttackDamageFork2, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroAttackDamage_fork3, 7, HeroAttackDamageFork3, 0, 0, 0 });
}

static void __fastcall compute_hero_attack_bonus(ICombatHeroInterface* combat_hero) {
    IHero* hero = combat_hero->hero();
    float bonus = 0.0f;
    int mastery = hero->skill_mastery(SKILL_COMBAT);
    if (mastery) bonus = mastery * constf_0_2;
    if (hero->has_artifact(ARTIFACT_CENTAUR_CROSSBOW)) bonus += constf_0_1;
    hero_attack_bonus = bonus;
}

// esi = attacking hero (ICombatHeroInterface)
__declspec(naked) void HeroAttackDamageFork() {
    __asm
    {
        mov ecx, esi
        call compute_hero_attack_bonus

        mov eax, dword ptr [esi - 0x9C]
        jmp [HeroAttackDamage_return]
    }
}

// st0 = damage factor: + bonus + 1
__declspec(naked) void HeroAttackDamageFork2() {
    __asm
    {
        fadd dword ptr [hero_attack_bonus]
        fadd dword ptr [constf_1]
        mov dword ptr [hero_attack_bonus], 0x0
        jmp [HeroAttackDamage_return2]
    }
}

// [esp+0x10] = damage: x (1 + bonus) when there is a bonus
__declspec(naked) void HeroAttackDamageFork3() {
    __asm
    {
        mov eax, dword ptr [hero_attack_bonus]
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
        jmp [HeroAttackDamage_return3]
    }
}
