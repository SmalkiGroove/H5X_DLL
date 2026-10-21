#include "pch.h"

// (199) Elrath's Divine Shield : a good morale roll of the hero's creatures sets their ATB to +0.2 more
// (DefaultStats +0xAD4, 0.4 -> 0.6).
// Patches func_CombatHero_ScheduleTimelineHook164 (0x00A6CF90, CombatUnit +0x164), the good morale effect: called by
// the turn end (0x00A6B468) when the good morale roll (+0x1CC) succeeds, and by the creature version 0x008A0DB0.
// Heroes never roll good morale (their +0x1CC returns false), so only creatures get there.

void ElrathsDivineShieldFork();

int ElrathsDivineShield_fork = 0x00A6D0C6;      // mov edx,[eax+0AD4h] (DefaultStats good morale ATB), pushed for set_atb (+0x18C)
int ElrathsDivineShield_return = 0x00A6D0CC;

static const float elraths_divine_shield_morale_atb_bonus = 0.2f;

void ElrathsDivineShield_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ElrathsDivineShield_fork, 6, ElrathsDivineShieldFork, 0, 0, 0 });
}

// self = `this` of the function (vbptr at this-0xB0), atb = DefaultStats good morale ATB
static float __fastcall elraths_divine_shield_morale_atb(char* self, int unused, float atb) {
    char* vbptr_holder = self - 0xB0;
    int** vbtable = *(int***)vbptr_holder;
    ICombatUnit* unit = (ICombatUnit*)(vbptr_holder + (int)vbtable[2]);
    IHero* hero = owner_hero(unit);
    return hero && hero->has_artifact(ARTIFACT_ELRATHS_DIVINE_SHIELD) ? atb + elraths_divine_shield_morale_atb_bonus : atb;
}

// esi = this, eax = DefaultStats; edx = ATB value (float bits) at return. edi / ebx are kept.
__declspec(naked) void ElrathsDivineShieldFork() {
    __asm
    {
        push dword ptr [eax + 0xAD4]
        mov ecx, esi
        call elraths_divine_shield_morale_atb             // st0 = ATB value, pops the float
        push eax
        fstp dword ptr [esp]
        pop edx
        jmp [ElrathsDivineShield_return]
    }
}
