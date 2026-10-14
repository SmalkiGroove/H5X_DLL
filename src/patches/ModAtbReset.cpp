#include "pch.h"
#include "types/Spells.h"
#include "Artfset.h"

// RESTORE ATB AFTER CASTING SPELL
// (126) Codex of the Saint : 15%
// Skill Elemental Focus : 10% (only for elemental spells)
// Academy set (15), 4 pieces + Academy hero : 15% of the ATB used saved, after the bonuses above (ArtfsetAcademy.cpp)

void AtbResetFork();

int AtbReset_fork = 0x00BBF8D1;    // hero ATB after a cast (Sorcery bonus at [esp+0x1C]), before the set bonuses: mov eax,[esi-7Ch] / lea edi,[esi-7Ch]
int AtbReset_return = 0x00BBF8D7;

void AtbReset_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, AtbReset_fork, 6, AtbResetFork, 0, 0, 0 });
}

// ATB after casting, from the vanilla Sorcery value.
static float __fastcall atb_after_cast(ICombatHero* combat_hero, int spell, float atb) {
    IHero* hero = combat_hero->hero();
    if (!hero) return atb;
    if (hero->has_artifact(ARTIFACT_CODEX_OF_THE_SAINT))
        atb += 0.15f;
    if (get_spell_element((int*)spell) != ELEMENT_NONE && hero->skill_mastery(PERK_ELEMENTAL_FOCUS) != 0)
        atb += 0.1f;
    return artfset_academy_hero_cast_atb(hero, atb);
}

// esi = hero CombatUnit subobject (ICombatHero), [ebx+4] = spell ID, ATB after the cast (float) at [esp+0x1C]
__declspec(naked) void AtbResetFork() {
    __asm
    {
        push dword ptr [esp + 0x1C]
        mov ecx, esi
        mov edx, dword ptr [ebx + 0x4]
        call atb_after_cast
        fstp dword ptr [esp + 0x1C]

        mov eax, dword ptr [esi - 0x7C]
        lea edi, dword ptr [esi - 0x7C]
        jmp [AtbReset_return]
    }
}
