#include "pch.h"
#include "types/Abilities.h"
#include "Artfset.h"

// ADD ABILITIES FROM OTHER SOURCES
// Perk STUN_TRAINING => Crushing Blow
// Perk ASSAULT_TRAINING => Violence
// Perk ARMOR_PROTECTION => Armored
// Perk BLOCK_CORRUPTION => Free Mind (Sandro's Cloak still removes it)
// Perk UNLIMITED_RETALIATIONS => Unlimited Retaliations
// Perk FIERCE_RETALIATIONS => Fierce Retaliations
// (142) Crag Hack's Helmet => Battle Rage
// (160) Legendary Boots => Rider Charge
// (182) Ranger's Cape => Agility
// (162) Dawn Gleam => Child of Light
// (163) Ardent Smith => Flame Wave
// (146) Barbarian Cape => removes Cowardice
// Artifact sets (Artfset.h) => Inferno: Liquid Flame Breath, Fire Shield; Necropolis: Undead, Regeneration;
//   Dungeon: Immunity to Mind Control (dark elves, after Sandro's Cloak); Dwarven: Presence of Commander, Aura of Bravery;
//   Orcs: Beat-up (Lizard Bite); Bandit: No Enemy Retaliation

// func_CombatCreature_HasAbility (0x008A34C0, CombatUnit +0x28C): ebp = creature CombatUnit subobject,
// esi = ability, edi = 0, ebx = hero whose Sandro's Cloak cancels mind immunity, [esp+0x1C] = owner IHero.

void CreatureAbilitiesFork();
void ArtfsetAbilityFork();

int CreatureAbilities_fork = 0x008A361C;     // after the forced lists, before the vanilla special cases: cmp esi,0A5h
int CreatureAbilities_return = 0x008A3622;   // jne: vanilla special cases

int CreatureAbilities_native = 0x008A3838;   // creature's own ability list
int CreatureAbilities_enable = 0x008A3688;   // return true
int CreatureAbilities_disable = 0x008A36B9;  // return false
int ArtfsetAbility_return = 0x008A383D;      // after the 5 bytes replaced at CreatureAbilities_native

void CreatureAbilities_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureAbilities_fork, 6, CreatureAbilitiesFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, CreatureAbilities_native, 5, ArtfsetAbilityFork, 0, 0, 0 });
}

enum AbilityOverride {
    ABILITY_VANILLA = 0,  // continue with the vanilla special cases
    ABILITY_NATIVE = 1,   // skip to the creature's own ability list
    ABILITY_GRANTED = 2,
    ABILITY_REMOVED = 3,
};

static bool owner_has_perk(ICombatCreature* unit, int perk) {
    ICombatHero* hero = unit->owner();
    return hero && hero->has_perk(perk);
}

// Granted by a perk of the owner hero
static int perk_ability(ICombatCreature* unit, int perk) {
    return owner_has_perk(unit, perk) ? ABILITY_GRANTED : ABILITY_VANILLA;
}

// Granted by an artifact of the owner hero
static int artifact_ability(IHero* own_hero, int artifact) {
    return own_hero && own_hero->has_artifact(artifact) ? ABILITY_GRANTED : ABILITY_VANILLA;
}

static int __fastcall creature_ability_override(ICombatCreature* unit, int ability, IHero* own_hero, IHero* sandro_hero) {
    switch (ability) {
    case ABILITY_CRUSHING_BLOW:          return perk_ability(unit, PERK_STUN_TRAINING);
    case ABILITY_BRUTALITY:              return perk_ability(unit, PERK_ASSAULT_TRAINING);       // Violence
    case ABILITY_ARMORED:                return perk_ability(unit, PERK_ARMOR_PROTECTION);
    case ABILITY_UNLIMITED_RETALIATION:  return perk_ability(unit, PERK_UNLIMITED_RETALIATIONS);
    case ABILITY_FIERCE_RETALIATION:     return perk_ability(unit, PERK_FIERCE_RETALIATIONS);
    case ABILITY_RIDER_CHARGE:           return artifact_ability(own_hero, ARTIFACT_LEGENDARY_BOOTS);
    case ABILITY_BATTLE_RAGE:            return artifact_ability(own_hero, ARTIFACT_BARBARIAN_HELMET);
    case ABILITY_AGILITY:                return artifact_ability(own_hero, ARTIFACT_RANGERS_CAPE);
    case ABILITY_BOND_OF_LIGHT:          return artifact_ability(own_hero, ARTIFACT_DAWN_GLEAM);           // Child of Light
    case ABILITY_FLAMEWAVE:              return artifact_ability(own_hero, ARTIFACT_ARDENT_SMITH);
    case ABILITY_COWARDICE:
        return own_hero && own_hero->has_artifact(ARTIFACT_BARBARIAN_CAPE) ? ABILITY_REMOVED : ABILITY_VANILLA;
    case ABILITY_IMMUNITY_TO_MIND_CONTROL:
        // replaces the vanilla case at 0x008A3694 (Sandro's Cloak), then adds Free Mind
        if (sandro_hero && sandro_hero->has_artifact(ARTIFACT_SANDROS_CLOAK)) return ABILITY_REMOVED;
        if (owner_has_perk(unit, PERK_CORRUPTION_IMMUNE)) return ABILITY_GRANTED;
        return ABILITY_NATIVE;
    }
    return ABILITY_VANILLA;
}

// Abilities granted by artifact sets, checked before the creature's own ability list (Artfset.h).
static int __fastcall artfset_has_ability(ICombatUnit* unit, int ability) {
    return artfset_inferno_ability(unit, ability) || artfset_necro_ability(unit, ability) || artfset_dungeon_ability(unit, ability)
        || artfset_dwarven_ability(unit, ability)
        || artfset_orcs_ability(unit, ability) || artfset_bandit_ability(unit, ability);
}

__declspec(naked) void CreatureAbilitiesFork() {
    __asm
    {
        push ebx                        // sandro_hero
        push dword ptr [esp + 0x20]     // own_hero ([esp+0x1C] before the first push)
        mov edx, esi
        mov ecx, ebp
        call creature_ability_override
        cmp eax, 0x2                    // ABILITY_GRANTED
        je ABILITIES_RETURN_TRUE
        cmp eax, 0x3                    // ABILITY_REMOVED
        je ABILITIES_RETURN_FALSE
        cmp eax, 0x1                    // ABILITY_NATIVE
        je ABILITIES_CHECK_NATIVE

        cmp esi, 0xA5                   // replayed: Hold Ground
        jmp [CreatureAbilities_return]

    ABILITIES_CHECK_NATIVE:
        jmp [CreatureAbilities_native]

    ABILITIES_RETURN_TRUE:
        jmp [CreatureAbilities_enable]

    ABILITIES_RETURN_FALSE:
        jmp [CreatureAbilities_disable]
    }
}

// Hook at CreatureAbilities_native (native ability lookup: mov ecx,[ebp-20h] / cmp ecx,edi).
__declspec(naked) void ArtfsetAbilityFork() {
    __asm
    {
        mov ecx, ebp
        mov edx, esi
        call artfset_has_ability
        test eax, eax
        je ARTFSET_ABILITY_NATIVE
        jmp [CreatureAbilities_enable]

    ARTFSET_ABILITY_NATIVE:
        mov ecx, dword ptr [ebp - 0x20]
        cmp ecx, edi
        jmp [ArtfsetAbility_return]
    }
}
