#include "pch.h"
#include "ElementsVirtuoso.h"

// ADD INITIATIVE FROM ADDITIONAL ARTIFACTS
// (131) Plunderer Shoes : +1 creatures init
// (57)  Boots of Swiftness : +2 creatures init
// (157) Harpy Boots : +1 creatures init
// (22)  Celestial Justicar Sword : +15% creature init
// (6)   Staff of Netherworld : -15% enemy creatures init
// (143) Skull Mask : -8% enemy creatures init
// (58)  Moon Blade : +1 hero init
// (118) Genji's Sandals : +1 hero init
// (107) Ring of Celerity : +2 hero init
// (65)  Ring of Caution : -2 hero init
// (201/202/203) Artificier : 5% creature init each, +0.5 hero init each
// Elements Virtuoso : +0.25 hero init per active element
// Knowledge stat : +0.08 hero init per point (+0.1 flat)

void CreatureInitiativeFork();
void CreatureInitlessFork();
void HeroInitiativeFork();

int CreatureInitiative_fork = 0x004BC2BF;
int CreatureInitiative_return = 0x004BC2D8;
int CreatureInitless_fork = 0x008A4621;
int CreatureInitless_return = 0x008A464B;
int HeroInitiative_fork = 0x00BC1F77;
int HeroInitiative_return = 0x00BC1F7D;

void Initiative_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureInitiative_fork, 25, CreatureInitiativeFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, CreatureInitless_fork, 42, CreatureInitlessFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroInitiative_fork, 6, HeroInitiativeFork, 0, 0, 0 });
}

// Flat creature initiative bonus, added to the base value before percent scaling.
static float __fastcall creature_init_flat_bonus(int* inventory) {
    float bonus = 0.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_PLUNDERER_SHOES))   bonus += 1.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_BOOTS_OF_SWIFTNESS)) bonus += 2.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_HARPY_BOOTS))       bonus += 1.0f;
    return bonus;
}

// Percent creature initiative bonus, returned in the accumulator the game keeps in esi.
// Replaces the vanilla Celestial Justicar Sword check (+20% -> +15%).
static int __fastcall creature_init_percent_bonus(int* inventory) {
    int percent = 0;
    if (count_equipped_artifact(inventory, ARTIFACT_CELESTIAL_JUSTICAR_SWORD)) percent += 15;
    percent += 5 * count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_1);
    percent += 5 * count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_2);
    percent += 5 * count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_3);
    return percent;
}

// Multiplier applied to enemy creatures initiative.
// Replaces the vanilla Staff of the Netherworld effect and adds the Skull Mask.
static float __fastcall creature_initless_multiplier(int* inventory) {
    float multiplier = 1.0f - 0.15f * count_equipped_artifact(inventory, ARTIFACT_STAFF_OF_THE_NETHERWORLD);
    multiplier *= 1.0f - 0.08f * count_equipped_artifact(inventory, ARTIFACT_SKULL_MASK);
    return multiplier;
}

// Flat hero initiative bonus.
// combatHero = CombatHero subobject (vtable 0x00E8499C); the AdvMap hero interface
// pointer is stored at [combatHero - 0x6C] (see docs/RE_hero_stats.md).
static float __fastcall hero_init_bonus(int* combatHero) {
    IHero* hero = (IHero*)combatHero[-(0x6C / 4)];
    int* inventory = hero->inventory();

    float bonus = 0.1f;
    if (count_equipped_artifact(inventory, ARTIFACT_MOONBLADE))        bonus += 1.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_RING_OF_CELERITY)) bonus += 2.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_GENJIS_SANDALS))   bonus += 1.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_RING_OF_CAUTION))  bonus -= 2.0f;
    if (count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_1))     bonus += 0.5f;
    if (count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_2))     bonus += 0.5f;
    if (count_equipped_artifact(inventory, ARTIFACT_ARTIFICIER_3))     bonus += 0.5f;

    bonus += 0.25f * elementVirtuoso_check(combatHero);

    // +0.08 per knowledge point, read from the real stat getter (AdvMap hero vtable +0x1C)
    // instead of the old mana/10 approximation (CombatHero vtable +0x238 * 0.008).
    bonus += 0.08f * hero->knowledge();

    return bonus;
}

// Hook at 0x004BC2BF, in the creature initiative calculation.
// Overwritten game code: inventory fetch + vanilla Celestial Justicar Sword check.
// At entry: edx = hero vtable, ecx = hero. At return the game expects
// edi = inventory, esi = percent bonus, flat initiative float at [esp+0x20].
__declspec(naked) void CreatureInitiativeFork() {
    __asm
    {
        call dword ptr [edx + 0x74]         // eax = hero inventory (replicates overwritten code)
        mov edi, eax                        // game code after the hook expects it in edi

        mov ecx, edi
        call creature_init_flat_bonus       // st0 = flat bonus
        fadd dword ptr ss : [esp + 0x20]
        fstp dword ptr ss : [esp + 0x20]

        mov ecx, edi
        call creature_init_percent_bonus    // eax = percent bonus
        mov esi, eax                        // game keeps the percent accumulator in esi

        jmp[CreatureInitiative_return]
    }
}

// Hook at 0x008A4621, in the enemy creature initiative calculation.
// Overwritten game code: vanilla Staff of the Netherworld multiplier.
// At entry: eax = enemy hero, edx = its vtable; initiative float at [esp+0x10].
__declspec(naked) void CreatureInitlessFork() {
    __asm
    {
        mov ecx, eax
        call dword ptr [edx + 0x74]         // eax = enemy hero inventory

        mov ecx, eax
        call creature_initless_multiplier   // st0 = combined multiplier
        fmul dword ptr ss : [esp + 0x10]
        fstp dword ptr ss : [esp + 0x10]

        jmp[CreatureInitless_return]
    }
}

// Hook at 0x00BC1F77, at the end of the hero initiative getter (vtable +0x1BC, impl 0x00BC1EB0).
// Overwritten game code: mov ecx,[esp+0xC] / test ecx,ecx (replicated before returning).
// At entry: esi = getter subobject; the CombatHero subobject is derived through the
// vbtable at [esi-0x118]. Hero initiative float at [esp+0x4].
__declspec(naked) void HeroInitiativeFork() {
    __asm
    {
        mov eax, dword ptr [esi - 0x118]
        mov ecx, dword ptr [eax + 0x8]
        lea ecx, dword ptr [ecx + esi - 0x118] // ecx = CombatHero subobject
        call hero_init_bonus                   // st0 = total flat bonus
        fadd dword ptr ss : [esp + 0x4]
        fstp dword ptr ss : [esp + 0x4]

        mov ecx, dword ptr ss : [esp + 0xC]    // replicate overwritten game code
        test ecx, ecx
        jmp[HeroInitiative_return]
    }
}
