#include "pch.h"

// Cold Death doubles the freeze duration

void ColdDeathFreezeFork();

int ColdDeathFreeze_fork = 0x004E66CE;
int ColdDeathFreeze_return = 0x004E66D4;

void ColdDeathFreeze_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ColdDeathFreeze_fork, 6, ColdDeathFreezeFork, 0, 0, 0 });
}

// target = frozen unit: the freeze lasts twice as long when the enemy hero (the caster's side) has Deadly Cold
static bool __fastcall cold_death_doubles_freeze(ICombatUnit* target) {
    IHero* hero = enemy_hero(target);
    return hero && hero->skill_mastery(PERK_DEADLY_COLD) != 0;
}

// edi = target unit, st0 = freeze duration
__declspec(naked) void ColdDeathFreezeFork() {
    __asm
    {
        fmul dword ptr [ebp + 0x7A4]

        mov ecx, edi
        call cold_death_doubles_freeze
        test al, al
        je COLD_DEATH_FREEZE_RETURN
        fmul dword ptr [constf_2]

    COLD_DEATH_FREEZE_RETURN:
        jmp [ColdDeathFreeze_return]
    }
}
