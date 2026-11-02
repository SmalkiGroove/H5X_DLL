#include "pch.h"

// STAFF OF THE SAINT GRANTS +7 EFFECTIVE SPELLPOWER FOR LIGHT MAGIC SPELLS

void StaffOfTheSaintFork();

int StaffOfTheSaint_fork = 0x00BC0473;
int StaffOfTheSaint_return = 0x00BC0479;

void StaffOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, StaffOfTheSaint_fork, 6, StaffOfTheSaintFork, 0, 0, 0 });
}

// combat_hero = CCombatHero (ICombatHeroInterface at +0xA0); notified = unit shown the artifact buff.
// Returns the spellpower bonus.
static int __fastcall staff_of_the_saint_bonus(char* combat_hero, int* notified) {
    IHero* hero = ((ICombatHeroInterface*)(combat_hero + 0xA0))->hero();
    if (!hero->has_artifact(ARTIFACT_STAFF_OF_THE_SAINT)) return 0;
    notify_artifact_buff(notified, 1, ARTIFACT_STAFF_OF_THE_SAINT, 0, 0);
    return 7;
}

// esi = CCombatHero, ebx = spellpower (also stored at [esp+0x10]), ebp = notified unit
__declspec(naked) void StaffOfTheSaintFork() {
    __asm
    {
        mov edx, ebp
        mov ecx, esi
        call staff_of_the_saint_bonus
        test eax, eax
        je STAFF_OF_THE_SAINT_RETURN
        add ebx, eax
        mov dword ptr ss: [esp + 0x10], ebx

    STAFF_OF_THE_SAINT_RETURN:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        jmp [StaffOfTheSaint_return]
    }
}
