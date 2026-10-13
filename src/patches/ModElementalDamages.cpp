#include "pch.h"
#include "ElementsVirtuoso.h"

// ADD ADDITIONAL ELEMENTAL DAMAGE ARTIFACTS
// (5) Titan's Trident : from +50% to +20% to lightning spells
// (18) Evercold Icicle : from +50% to +20% to frost spells
// (32) Solar Ring : from +50% to +20% to fire spells
// (61) Emerald Slippers : from +50% to +20% to earth spells
// (192) Orb of Air Affinity : +33% to lightning spells
// (193) Orb of Earth Affinity : +33% to earth spells
// (194) Orb of Fire Affinity : +33% to fire spells
// (195) Orb of Water Affinity : +33% to frost spells
// (102) Altar of Elements Shard : +10% to all element spells
// Skill Obliteration (149) : +30% to all element spells
// Skill Elements Virtuoso (235) : +10% scaling when rotating elements

void ElemDamageFork();
void ElemDamageForkAlt();

int ElemDamage_fork = 0x00977086;     // after the vanilla element artifacts (hero caster): cmp [esp+10h],0 / jle 0x0097712F
int ElemDamage_return = 0x00977091;
int ElemDamage_end = 0x0097712F;

void ElemDamage_init(pugi::xml_document& doc) {
    // caster without hero / adventure hero: skip the bonuses, only replay the final check
    assembly_patches.push_back({ HOOK_JE, 0x00976EFE, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F1F, 6, ElemDamageForkAlt, 0, 0, 0 });
    assembly_patches.push_back({ HOOK_JE, 0x00976F3B, 6, ElemDamageForkAlt, 0, 0, 0 });
    // vanilla element artifacts: fmul [0x00E3A3AC] (x1.5) -> fmul [0x00E12F1C]
    assembly_patches.push_back({ PATCH_WRTE, 0x00976F75, 6, nullptr, 0, 0, 0, "D80D1C2FE100" }); // Titan's Trident
    assembly_patches.push_back({ PATCH_WRTE, 0x00976FC4, 6, nullptr, 0, 0, 0, "D80D1C2FE100" }); // Evercold Icicle
    assembly_patches.push_back({ PATCH_WRTE, 0x00977013, 6, nullptr, 0, 0, 0, "D80D1C2FE100" }); // Solar Ring
    assembly_patches.push_back({ PATCH_WRTE, 0x00977062, 6, nullptr, 0, 0, 0, "D80D1C2FE100" }); // Emerald Slippers
    assembly_patches.push_back({ PATCH_HOOK, ElemDamage_fork, 11, ElemDamageFork, 0, 0, 0 });
}

// The game scales the damage with x87 math (extended precision, truncated): done in double here with the same
// single precision factors so that the truncated results match.
static void scale_damage(int* damage, double factor) {
    *damage = (int)(*damage * factor);
}

static int element_orb(int element) {
    switch (element) {
    case 1: return ARTIFACT_ORB_OF_AIR;    // lightning
    case 2: return ARTIFACT_ORB_OF_FIRE;
    case 3: return ARTIFACT_ORB_OF_WATER;  // ice
    case 4: return ARTIFACT_ORB_OF_EARTH;
    }
    return 0;
}

// caster = hero CombatUnit subobject, inventory = its adventure hero inventory, damage = the game's damage value.
// Each bonus is applied in turn and added to the spell tooltip (notify_*_buff), like the vanilla ones.
static void __fastcall elemental_damage_bonuses(ICombatHero* caster, int* inventory, int spell, int* notify_context, int* damage) {
    int element = get_spell_element((int*)spell);
    int orb = element_orb(element);
    if (!orb) return;  // not an element spell

    if (count_equipped_artifact(inventory, orb)) {
        scale_damage(damage, 4.0f / 3.0f);
        notify_artifact_buff(notify_context, 1, orb, 0, 0);
    }
    if (count_equipped_artifact(inventory, ARTIFACT_ALTAR_OF_ELEMENTS_SHARD)) {
        scale_damage(damage, 1.1f);
        notify_artifact_buff(notify_context, 1, ARTIFACT_ALTAR_OF_ELEMENTS_SHARD, 0, 0);
    }
    if (caster->has_perk(PERK_OBLITERATION)) {
        scale_damage(damage, 1.3f);
        notify_skill_buff(notify_context, 1, PERK_OBLITERATION, 0, 0);
    }
    int elements = elementVirtuoso_check(caster);
    if (elements) {
        scale_damage(damage, 1.0 + elements * (double)0.1f);
        notify_skill_buff(notify_context, 1, PERK_ELEMENTS_VIRTUOSO, 0, 0);
    }
}

// esi = caster (hero CombatUnit subobject), edi = adventure hero inventory, [ebp+4] = spell,
// ebx = notify context, damage int at [esp+0x10]
__declspec(naked) void ElemDamageFork() {
    __asm
    {
        lea eax, dword ptr [esp + 0x10]
        push eax                        // damage
        push ebx                        // notify_context
        push dword ptr [ebp + 0x4]      // spell
        mov edx, edi
        mov ecx, esi
        call elemental_damage_bonuses
        jmp ElemDamageForkAlt
    }
}

// Replayed game code at 0x00977086: no damage left -> skip the rest
__declspec(naked) void ElemDamageForkAlt() {
    __asm
    {
        cmp dword ptr [esp + 0x10], 0x0
        jle ELEMDAMAGE_END
        jmp [ElemDamage_return]
    ELEMDAMAGE_END:
        jmp [ElemDamage_end]
    }
}
