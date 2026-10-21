#include "pch.h"
#include "types/Spells.h"

// ADD SPELL IMMUNITIES FROM ADDITIONAL ARTIFACTS
// (184) Forgotten Ring : Immune to Holy Word (35), replaces the vanilla Ring of the Unrepentant (70)
// (180) Moonlight Band : Immune to Unholy Word (21)
// (123) Obsidian Armor : Immune to Armageddon (10)
// (172) Sanctuary's Crown : Immune to Hypnotize (20)
// (115) Cloak of the Saint : Immune to Plague (14)
// (165) Shantiri Breastplate : Immune to Earthquake (41)
// (50)  Celestial Justicar Helmet : Immune to Forgetfulness (17) and Berserk (18), on top of its vanilla Blind (19)
// Vanilla immunities kept: (48) Shantiri Moon Disc : Implosion (9), (49) Plumed Boots : Slow (12),
//       (51) Helm of the Dwarven Kings : Berserk (18)

void SpellImmuneFork();
void HolyWordImmuneFork();

// Artifact immunity checks of the spell immunity function: ebx = spell, eax = inventory of the target's hero.
// Vanilla cases follow (spells 9 / 0xC / 0x13 / 0x12 -> artifacts 48 / 49 / 50 / 51), each one pushing
// (artifact, 3, 0) for the shared "immune by artifact" code.
int SpellImmune_fork = 0x0097CE1E;      // cmp ebx,9 / mov ecx,eax
int SpellImmune_return = 0x0097CE23;    // jne (next vanilla case)
int SpellImmune_apply = 0x0097CE7E;     // immune by artifact (args pushed), hit chance = 100

// Holy Word case: eax = inventory of the target's hero, count result tested at the return.
int HolyWordImmune_fork = 0x0097CC2C;   // push 46h / mov ecx,eax / call count_equipped_artifact
int HolyWordImmune_return = 0x0097CC35; // test eax,eax

void SpellImmune_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpellImmune_fork, 5, SpellImmuneFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HolyWordImmune_fork, 9, HolyWordImmuneFork, 0, 0, 0 });
}

// Returns the artifact that grants the immunity, or 0 to go on with the vanilla cases.
static int __fastcall spell_immune_artifact(int* inventory, int spell) {
    int artifact;
    switch (spell) {
    case SPELL_ARMAGEDDON:      artifact = ARTIFACT_OBSIDIAN_ARMOR; break;
    case SPELL_HYPNOTIZE:       artifact = ARTIFACT_SANCTUARYS_CROWN; break;
    case SPELL_UNHOLY_WORD:     artifact = ARTIFACT_MOONLIGHT_BAND; break;
    case SPELL_EARTHQUAKE:      artifact = ARTIFACT_SHANTIRI_BREASTPLATE; break;
    case SPELL_PLAGUE:          artifact = ARTIFACT_CLOAK_OF_THE_SAINT; break;
    case SPELL_FORGETFULNESS:
    case SPELL_BERSERK:         artifact = ARTIFACT_CELESTIAL_JUSTICAR_HELMET; break;
    default:                    return 0;
    }
    return count_equipped_artifact(inventory, artifact) ? artifact : 0;
}

static int __fastcall holy_word_immune(int* inventory) {
    return count_equipped_artifact(inventory, ARTIFACT_FORGOTTEN_RING);
}

__declspec(naked) void SpellImmuneFork() {
    __asm
    {
        push eax                        // inventory
        mov ecx, eax
        mov edx, ebx
        call spell_immune_artifact
        test eax, eax
        jne SPELL_IMMUNE_APPLY

        pop eax                         // vanilla cases: replicate overwritten game code
        cmp ebx, 0x9
        mov ecx, eax
        jmp [SpellImmune_return]

    SPELL_IMMUNE_APPLY:
        add esp, 0x4
        push 0
        push 3
        push eax                        // artifact
        jmp [SpellImmune_apply]
    }
}

__declspec(naked) void HolyWordImmuneFork() {
    __asm
    {
        mov ecx, eax
        call holy_word_immune
        jmp [HolyWordImmune_return]
    }
}
