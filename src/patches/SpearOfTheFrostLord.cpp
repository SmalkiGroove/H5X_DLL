#include "pch.h"

// Spear of the frost lord (170) ad 20% bonus ice damage
// Bonus ice damage of creature attacks: Cold Steel 10% (vanilla constant replaced) + Spear of the Frost Lord 20%.

void SpearOfTheFrostLordFork();
void SpearOfTheFrostLordFork2();

int SpearOfTheFrostLord_fork = 0x00A5C016;
int SpearOfTheFrostLord_return = 0x00A5C02C;
int SpearOfTheFrostLord_fork2 = 0x00A5C090;
int SpearOfTheFrostLord_return2 = 0x00A5C096;

void SpearOfTheFrostLord_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpearOfTheFrostLord_fork, 22, SpearOfTheFrostLordFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, SpearOfTheFrostLord_fork2, 6, SpearOfTheFrostLordFork2, 0, 0, 0 });
}

static float frost_damage_bonus = 0.0f;   // set by the first hook, used by the second

// combat_hero = the attacker's hero (ICombatHeroInterface, not null), attacker = attacking unit
static void __fastcall compute_frost_damage_bonus(int* combat_hero, ICombatUnit* attacker) {
    float bonus = 0.0f;
    if (combat_creature_detail::combat_hero_from_outer(combat_hero)->has_perk(PERK_COLD_STEEL)) bonus += constf_0_1;
    IHero* hero = owner_hero(attacker);
    if (hero && hero->has_artifact(ARTIFACT_SPEAR_OF_THE_FROST_LORD)) bonus += constf_0_2;
    frost_damage_bonus = bonus;
}

// eax = attacker's hero (ICombatHeroInterface), ebx = attacking unit; the game returns 0 when the bonus is 0
__declspec(naked) void SpearOfTheFrostLordFork() {
    __asm
    {
        mov edx, ebx
        mov ecx, eax
        call compute_frost_damage_bonus
        mov eax, dword ptr [frost_damage_bonus]
        test eax, eax
        jmp [SpearOfTheFrostLord_return]
    }
}

// st0 = damage: x bonus
__declspec(naked) void SpearOfTheFrostLordFork2() {
    __asm
    {
        fmul dword ptr [frost_damage_bonus]
        jmp [SpearOfTheFrostLord_return2]
    }
}
