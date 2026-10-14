#include "pch.h"
#include "Artfset.h"

// EDIT DAMAGE OUTPUT MODIFIERS FROM ARTIFACTS
// (132) Butcher Glaive : +15% melee damage
// (129) Avenger Bow : +10% ranged damage
// (190) Celestial Justicar Armor : -25% melee damage
// (122) Sentinel's Shield : -10% ranged damage
// (133) Celestial Justicar Shield : -50% ranged damage
// (198) Shield of Dwarven Kings : -30% ranged damage
// Skill Courage reduce all damages per Morale point
// Skill Arcane Protection reduce all damages by 0.5% per spellpower
// Academy set (15), 6 pieces + Academy hero : -20% damage for mages and their adjacent allies (ArtfsetAcademy.cpp)

void MeleeDamageInputFork();
void RangedDamageInputFork();
void MeleeDamageOutputFork();
void RangedDamageOutputFork();
void AllDamageOutputFork();

int MeleeDamageInput_fork = 0x00A58E74;    // attacker hero bonuses, melee branch: mov edx,[ebp] / mov ecx,ebp
int MeleeDamageInput_return = 0x00A58E79;
int RangedDamageInput_fork = 0x00A58EBD;   // attacker hero bonuses, ranged branch: mov eax,[ebp+4] / mov ecx,[eax+8]
int RangedDamageInput_return = 0x00A58EC3;
int MeleeDamageOutput_fork = 0x00A5AC59;   // func_ComputeDamageMitigation, melee branch: mov eax,[edi] / mov ecx,edi / call [eax]
int MeleeDamageOutput_return = 0x00A5AC5F;
int RangedDamageOutput_fork = 0x00A5ACF9;  // func_ComputeDamageMitigation, ranged branch: mov edx,[edi+4] / mov eax,[edx+8]
int RangedDamageOutput_return = 0x00A5ACFF;
int AllDamageOutput_fork = 0x00A5AD6E;     // func_ComputeDamageMitigation, after both branches: mov edx,[ebx] / mov ecx,ebx / call [edx+0Ch]
int AllDamageOutput_return = 0x00A5AD75;

void DamageOutput_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, MeleeDamageInput_fork, 5, MeleeDamageInputFork, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, RangedDamageInput_fork, 6, RangedDamageInputFork, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, MeleeDamageOutput_fork, 6, MeleeDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, RangedDamageOutput_fork, 6, RangedDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AllDamageOutput_fork, 7, AllDamageOutputFork, 0, 0, 0 });
}

// The hooks get the hero as the game's NWorld::ICombatHero interface (complete object +0xA0, vbptr at +4),
// converted here to the repo's ICombatHero (CombatUnit subobject +0x11C). Null stays null.
static ICombatHero* combat_hero(int* combat_hero_interface) {
    return combat_creature_detail::combat_hero_from_outer(combat_hero_interface);
}

// Attacker side: multipliers of the attacker's damage, hero = attacker's hero (not null).
static float __fastcall melee_input_multiplier(int* hero_interface) {
    return combat_hero(hero_interface)->has_artifact(ARTIFACT_BUTCHER_GLAIVE) ? 1.15f : 1.0f;
}

static float __fastcall ranged_input_multiplier(int* hero_interface) {
    return combat_hero(hero_interface)->has_artifact(ARTIFACT_AVENGER_BOW) ? 1.1f : 1.0f;
}

// Defender side: multipliers of the damage taken, hero = defender's hero.
static float __fastcall melee_output_multiplier(int* hero_interface) {
    return combat_hero(hero_interface)->has_artifact(ARTIFACT_CELESTIAL_JUSTICAR_ARMOR) ? 0.75f : 1.0f;
}

static float __fastcall ranged_output_multiplier(int* hero_interface) {
    ICombatHero* hero = combat_hero(hero_interface);
    float multiplier = 1.0f;
    if (hero->has_artifact(ARTIFACT_SENTINELS_SHIELD))            multiplier *= 0.9f;
    if (hero->has_artifact(ARTIFACT_CELESTIAL_JUSTICAR_SHIELD))   multiplier *= 0.5f;
    if (hero->has_artifact(ARTIFACT_SHIELD_OF_THE_DWARVEN_KINGS)) multiplier *= 0.7f;
    return multiplier;
}

// hero = defender's hero (may be 0), defender = defending unit (CombatUnit subobject)
static float __fastcall all_output_multiplier(int* hero_interface, ICombatUnit* defender) {
    float multiplier = 1.0f;
    ICombatHero* hero = combat_hero(hero_interface);
    if (!hero) return multiplier;

    // Courage: -mastery% per morale point of the defender (negative morale increases the damage)
    IHero* adventure_hero = hero->hero();
    int courage = adventure_hero ? adventure_hero->skill_mastery(SKILL_COURAGE) : 0;
    if (courage != 0)
        multiplier *= 1.0f - courage * 0.01f * defender->morale();

    // Arcane Protection: -0.5% per point of the hero's total spellpower (no spell context)
    if (hero->has_perk(PERK_ARCANE_PROTECTION)) {
        int half_spellpower = hero->spellpower() >> 1;
        if (half_spellpower != 0)
            multiplier *= 1.0f - half_spellpower * 0.01f;
    }

    multiplier *= artfset_academy_damage_taken(defender);
    return multiplier;
}

// Attacker damage multiplier at [esp+0x38], ebp = attacker's hero.
__declspec(naked) void MeleeDamageInputFork() {
    __asm
    {
        mov ecx, ebp
        call melee_input_multiplier
        fmul dword ptr [esp + 0x38]
        fstp dword ptr [esp + 0x38]
        mov edx, dword ptr [ebp]
        mov ecx, ebp
        jmp [MeleeDamageInput_return]
    }
}

__declspec(naked) void RangedDamageInputFork() {
    __asm
    {
        mov ecx, ebp
        call ranged_input_multiplier
        fmul dword ptr [esp + 0x38]
        fstp dword ptr [esp + 0x38]
        mov eax, dword ptr [ebp + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        jmp [RangedDamageInput_return]
    }
}

// func_ComputeDamageMitigation (0x00A5A900, ret 14h): result = arg1 ([esp+0x18] here) * arg4 ([esp+0x24]),
// every vanilla reduction scales arg1. edi = defender's hero (0 on the path to AllDamageOutput when there is
// none), ebp = defending unit, ebx = attacking unit, byte [esp+0x20] = ranged.
__declspec(naked) void MeleeDamageOutputFork() {
    __asm
    {
        mov ecx, edi
        call melee_output_multiplier
        fmul dword ptr [esp + 0x18]
        fstp dword ptr [esp + 0x18]
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        jmp [MeleeDamageOutput_return]
    }
}

__declspec(naked) void RangedDamageOutputFork() {
    __asm
    {
        mov ecx, edi
        call ranged_output_multiplier
        fmul dword ptr [esp + 0x18]
        fstp dword ptr [esp + 0x18]
        mov edx, dword ptr [edi + 0x4]
        mov eax, dword ptr [edx + 0x8]
        jmp [RangedDamageOutput_return]
    }
}

__declspec(naked) void AllDamageOutputFork() {
    __asm
    {
        mov ecx, edi
        mov edx, ebp
        call all_output_multiplier
        fmul dword ptr [esp + 0x18]
        fstp dword ptr [esp + 0x18]
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0xC]
        jmp [AllDamageOutput_return]
    }
}
