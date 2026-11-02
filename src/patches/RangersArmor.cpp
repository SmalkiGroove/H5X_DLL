#include "pch.h"

// RANGER'S ARMOR GRANTS +10 DEF VS LARGE CREATURES

void RangersArmorFork();

int RangersArmor_fork = 0x00A5AA83;
int RangersArmor_return = 0x00A5AA8A;

void RangersArmor_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, RangersArmor_fork, 6, RangersArmorFork, 0, 0, 0 });
}

// Defense bonus of the defender against the attacker: 0x10 against large creatures when the defender's own hero
// wears the Ranger's Cuirass
static int __fastcall rangers_armor_bonus(ICombatUnit* defender, ICombatUnit* attacker) {
    IHero* hero = owner_hero(defender);
    if (!hero || !hero->has_artifact(ARTIFACT_RANGERS_CUIRASS)) return 0;
    return attacker->size() > 1 ? 0x10 : 0;
}

// ebp = defending unit, ebx = attacking unit, [esp+0xC] = defense
__declspec(naked) void RangersArmorFork() {
    __asm
    {
        mov edx, ebx
        mov ecx, ebp
        call rangers_armor_bonus
        add dword ptr ss: [esp + 0xC], eax

        mov edx, dword ptr [ebx]
        push 0x9F
        jmp [RangersArmor_return]
    }
}
