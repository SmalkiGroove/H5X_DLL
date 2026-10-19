#include "pch.h"

// (87) Tarot Deck :
// - the hero's damaging spells can be lucky, like the Lucky Caster perk (PERK_LUCKY_CASTER, 142)
// - +1 hero luck for the lucky spell roll only (before the [-5, 5] clamp)
// Patches the Lucky Caster check of the hero spell cast function (0x00983790): for a damaging spell cast by a hero,
// the game rolls the caster's luck (CombatUnit +0x1D0, hero impl 0x00A6B820) and uses the multiplier on the spell
// damage. The luck bonus is a flag set around that roll call and added where the hero luck roll reads the luck.

void TarotDeckFork();
void TarotDeckLuckFork();

int TarotDeck_fork = 0x00983AD6;        // push 8Eh / mov ecx,edi / call [edx+290h] (caster has_skill(PERK_LUCKY_CASTER)),
                                        // test al,al / je, then the luck roll mov eax,[edi] / mov ecx,edi / call [eax+1D0h]
int TarotDeck_return = 0x00983AF5;      // fst [esp+18h]: st0 = luck multiplier
int TarotDeck_skip = 0x00983B74;        // je target: no luck roll
int TarotDeckLuck_fork = 0x00A6B841;    // hero luck roll, after the luck getter (+0x1AC): cmp eax,5 / jl
int TarotDeckLuck_max = 0x00A6B846;     // mov eax,5
int TarotDeckLuck_return = 0x00A6B84D;  // cmp eax,-5

static int tarot_spell_luck_bonus = 0;  // only non-zero during the spell luck roll of a Tarot Deck hero

void TarotDeck_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, TarotDeck_fork, 31, TarotDeckFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, TarotDeckLuck_fork, 5, TarotDeckLuckFork, 0, 0, 0 });
}

// caster = hero CombatUnit subobject (the game already checked the +0x74 hero subobject, which creatures lack)
static int __fastcall tarot_deck_lucky_caster(ICombatHero* caster) {
    return caster->has_perk(PERK_LUCKY_CASTER) || caster->has_artifact(ARTIFACT_TAROT_DECK);
}

static int __fastcall tarot_deck_luck_bonus(ICombatHero* caster) {
    return caster->has_artifact(ARTIFACT_TAROT_DECK) ? 1 : 0;
}

// edi = caster
__declspec(naked) void TarotDeckFork() {
    __asm
    {
        mov ecx, edi
        call tarot_deck_lucky_caster
        test eax, eax
        je TAROT_NO_ROLL

        mov ecx, edi
        call tarot_deck_luck_bonus
        mov tarot_spell_luck_bonus, eax

        mov eax, dword ptr [edi]                // replicate overwritten game code: caster luck roll
        mov ecx, edi
        call dword ptr [eax + 0x1D0]            // st0 = luck multiplier

        mov tarot_spell_luck_bonus, 0
        jmp [TarotDeck_return]

    TAROT_NO_ROLL:
        jmp [TarotDeck_skip]
    }
}

// eax = hero luck (unclamped)
__declspec(naked) void TarotDeckLuckFork() {
    __asm
    {
        add eax, tarot_spell_luck_bonus
        cmp eax, 5                              // replicate overwritten game code
        jl TAROT_LUCK_BELOW_MAX
        jmp [TarotDeckLuck_max]

    TAROT_LUCK_BELOW_MAX:
        jmp [TarotDeckLuck_return]
    }
}
