#include "pch.h"

// (43) Dragon Flame Tongue : the hero's creatures attacks deal 15% additional fire damage (added to Fiery Wrath).
// Replaces the PERK_FIERY_WRATH block of func_GetAdditionalFireDmg (0x00A5ED90): for a creature attacker, the
// bonus fire damage is FUN_00a5bbf0(1) (attack damage) x [0x00E12F10] (Fiery Wrath, 0.1).

void DragonFlameTongueFork();

int DragonFlameTongue_fork = 0x00A5EDDE;      // attacker is a creature: side hero has_skill(PERK_FIERY_WRATH) ...
int DragonFlameTongue_return = 0x00A5EE17;    // ... up to fstp [esp+0Ch] (bonus fire damage, float)
int DragonFlameTongue_damage = 0x00A5BBF0;    // fastcall(ecx = [esp+4Ch], edx = target, push 1): attack damage

static const float* fiery_wrath_fraction = (const float*)0x00E12F10;
static const float dragon_flame_tongue_fraction = 0.15f;

void DragonFlameTongue_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, DragonFlameTongue_fork, 57, DragonFlameTongueFork, 0, 0, 0 });
}

// hero_interface = attacker's side hero (NWorld::ICombatHero, vbptr at +4, not null)
static float __fastcall bonus_fire_fraction(int* hero_interface) {
    ICombatHero* hero = combat_creature_detail::combat_hero_from_outer(hero_interface);
    float fraction = 0.0f;
    if (hero->has_perk(PERK_FIERY_WRATH))                 fraction += *fiery_wrath_fraction;
    if (hero->has_artifact(ARTIFACT_DRAGON_FLAME_TONGUE)) fraction += dragon_flame_tongue_fraction;
    return fraction;
}

// edi = side hero, ebp = target, [esp+0xC] = bonus fire damage (0.0), [esp+0x4C] = arg of the damage function
__declspec(naked) void DragonFlameTongueFork() {
    __asm
    {
        mov ecx, edi
        call bonus_fire_fraction
        fstp dword ptr [esp + 0xC]
        cmp dword ptr [esp + 0xC], 0            // 0.0f: no bonus
        je DRAGON_FLAME_TONGUE_RETURN

        mov ecx, dword ptr [esp + 0x4C]         // replicate the game's damage call
        push 1
        mov edx, ebp
        call [DragonFlameTongue_damage]
        mov dword ptr [esp + 0x4C], eax
        fild dword ptr [esp + 0x4C]
        fmul dword ptr [esp + 0xC]
        fstp dword ptr [esp + 0xC]

    DRAGON_FLAME_TONGUE_RETURN:
        jmp [DragonFlameTongue_return]
    }
}
