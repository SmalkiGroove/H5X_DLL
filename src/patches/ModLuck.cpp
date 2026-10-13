#include "pch.h"
#include "types/HeroSpecs.h"

// ADD LUCK BUFFS FROM ADDITIONAL SOURCES
// skill Avenger +mastery Luck
// skill Precision +2 Luck

// ADD LUCK DEBUFFS FROM ADDITIONAL SOURCES
// (63) Cursed Ring : change from -2 to -1 Luck
// (175) Spirit of Oppression : -1 Luck
// (99) Misfortune Dices : -2 Luck
// (164) Cloak of Darkness : -1 Luck
// also skill Dead Man's Curse -3 Luck
// spec Eruina (Matron Salvo) -1 Luck

// (187) Dragonsbane : the hero's creatures never roll luck

void LuckBuffFork();
void LuckDebuffFork();
void LuckToggleFork();

int LuckBuff_fork = 0x00B4CA2B;      // IHero get_luck_raw (0x00B4CA10), after the luck skill: mov edi,[esi+228h]
int LuckBuff_return = 0x00B4CA31;
int LuckDebuff_fork = 0x00989E4E;    // func_ComputeLuckModifiers, enemy hero debuffs, vanilla Cursed Ring: neg eax / lea esi,[esi+eax*2]
int LuckDebuff_return = 0x00989E70;
int LuckToggle_fork = 0x008A3F0F;    // creature luck roll (CombatUnit +0x1D0, 0x008A3EF0): mov edi,[esi-94h]
int LuckToggle_return = 0x008A3F15;
int LuckToggle_end = 0x008A3F3B;     // fld 1.0 (no luck); pop edi; pop esi; pop ecx; ret

void Luck_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x00B4CA1E, 1, nullptr, SKILL_AVENGER, 0, 0, 0 });  // push 5 (Luck) -> Avenger mastery
    assembly_patches.push_back({ PATCH_HOOK, LuckBuff_fork, 6, LuckBuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LuckDebuff_fork, 34, LuckDebuffFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LuckToggle_fork, 6, LuckToggleFork, 0, 0, 0 });
}

// Luck from the hero's own skills, added to the hero's base luck.
static int __fastcall hero_luck_bonus(IHero* hero) {
    return hero->skill_mastery(PERK_PRECISION) > 0 ? 2 : 0;
}

// Luck removed from a unit by the enemy hero (NWorld::ICombatHero interface, not null).
// Replaces the vanilla enemy hero debuffs from the Cursed Ring on (-2 -> -1).
static int __fastcall enemy_luck_malus(int* enemy_hero_interface) {
    ICombatHero* enemy = combat_creature_detail::combat_hero_from_outer(enemy_hero_interface);
    int malus = 0;
    IHero* hero = enemy->hero();
    if (hero) {
        int* inventory = hero->inventory();
        malus += count_equipped_artifact(inventory, ARTIFACT_CURSED_RING);
        malus += count_equipped_artifact(inventory, ARTIFACT_SPIRIT_OF_OPPRESSION);
        malus += 2 * count_equipped_artifact(inventory, ARTIFACT_CURSED_DICES);
        malus += count_equipped_artifact(inventory, ARTIFACT_CLOAK_OF_DARKNESS);
    }
    if (enemy->has_perk(ULTI_DEAD_MANS_CURSE))       malus += 3;
    if (enemy->has_spec(HERO_SPEC_MATRON_SALVO))     malus += 1;  // Eruina
    return malus;
}

// Dragonsbane: the creature's own hero disables its luck rolls.
static int __fastcall luck_disabled(ICombatCreature* unit) {
    ICombatHero* hero = unit->owner();
    return hero && hero->has_artifact(ARTIFACT_DRAGONSBANE);
}

// esi = IHero, ebx = luck being summed
__declspec(naked) void LuckBuffFork() {
    __asm
    {
        mov ecx, esi
        call hero_luck_bonus
        add ebx, eax

        mov edi, dword ptr [esi + 0x228]
        jmp [LuckBuff_return]
    }
}

// edi = enemy hero (NWorld::ICombatHero interface), esi = luck being summed
__declspec(naked) void LuckDebuffFork() {
    __asm
    {
        mov ecx, edi
        call enemy_luck_malus
        sub esi, eax
        jmp [LuckDebuff_return]
    }
}

// esi = creature CombatUnit subobject, eax = game value still needed after the return
__declspec(naked) void LuckToggleFork() {
    __asm
    {
        push eax
        mov ecx, esi
        call luck_disabled
        test eax, eax
        pop eax
        jne LUCK_DISABLED

        mov edi, dword ptr [esi - 0x94]
        jmp [LuckToggle_return]

    LUCK_DISABLED:
        jmp [LuckToggle_end]
    }
}
