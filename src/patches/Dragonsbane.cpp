#include "pch.h"

// (187) Dragonsbane : ignore defender SCreature base defense (keep hero/buff bonuses)

void DragonsbaneFork();

int Dragonsbane_fork = 0x00A5A925;
int Dragonsbane_return = 0x00A5A92E;

// defender_outer = combat unit outer (vtable at [outer+0]), same as ebp in DragonsbaneFork.
// func_GetUnitDefense core (0x008A22F0) is called with ECX = outer - 0x148 (0x008A32DB).
static constexpr int kCombatUnitOuterToDefenseCore = 0x148;

static int get_creature_base_defense(int defender_outer) {
    if (!defender_outer)
        return 0;

    if (((ICombatUnit*)defender_outer)->unit_kind() != 1)   // 1 = creature
        return 0;

    const int unit_core = defender_outer - kCombatUnitOuterToDefenseCore;
    void* creature_stack = *(void**)(unit_core + 0xA8); // [outer - 0xA0], adv-map stack ptr
    if (!creature_stack)
        return 0;

    const int type_id = *(int*)((char*)creature_stack + 0x1C);
    int* creature_data = get_creature_data(type_id);
    if (!creature_data)
        return 0;

    return *(int*)((char*)creature_data + 0x48); // SCreature.Defense
}

static int apply_dragonsbane_defense(int defender_outer, int total_defense) {
    int base = get_creature_base_defense(defender_outer);
    if (base <= 0)
        return total_defense;
    int mitigated = total_defense - base;
    return mitigated > 0 ? mitigated : 0;
}

// attacker = attacking unit: with Dragonsbane on its own hero, the defense loses the creature's base defense
static int __fastcall dragonsbane_defense(ICombatUnit* attacker, int defender_outer, int total_defense) {
    IHero* hero = owner_hero(attacker);
    if (!hero || !hero->has_artifact(ARTIFACT_DRAGONSBANE)) return total_defense;
    return apply_dragonsbane_defense(defender_outer, total_defense);
}

void Dragonsbane_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Dragonsbane_fork, 9, DragonsbaneFork, 0, 0, 0 });
}

// ebx = attacking unit, ebp = defending unit, [esp+4] = defense
__declspec(naked) void DragonsbaneFork() {
    __asm
    {
        push dword ptr [esp + 0x4]
        mov edx, ebp
        mov ecx, ebx
        call dragonsbane_defense
        mov dword ptr [esp + 0x4], eax

        mov eax, dword ptr [ebx]
        mov ecx, ebx
        push 0x8A
        jmp [Dragonsbane_return]
    }
}
