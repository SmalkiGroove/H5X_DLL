#include "pch.h"

// (161) Golden Goose : lucky strikes of the hero's units deal +33% of the base attack damage
// (good luck multiplier + 0.33, e.g. x2.0 -> x2.33).
// Hooks func_GetGoodLuckMultiplier (0x00A6A680), called on every successful good luck roll by the hero luck roll
// (0x00A6B8B1) and by the creature one (0x008A4030, Supernatural Luck / forced good luck path). It returns
// DefaultStats +0xACC, or +0x6A8 when the unit's hero has skill 0x74.

void GoldenGooseFork();

int GoldenGoose_fork = 0x00A6A680;      // mov eax,[ecx+4] / mov edx,[eax+8]
int GoldenGoose_return = 0x00A6A686;

static const float golden_goose_bonus = 0.33f;

void GoldenGoose_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, GoldenGoose_fork, 6, GoldenGooseFork, 0, 0, 0 });
}

// luck_this = `this` of func_GetGoodLuckMultiplier (vbptr at this+4): the virtual base it reaches is the unit.
static float __fastcall golden_goose_multiplier(char* luck_this, int unused, float multiplier) {
    char* vbptr_holder = luck_this + 4;
    int** vbtable = *(int***)vbptr_holder;
    ICombatUnit* unit = (ICombatUnit*)(vbptr_holder + (int)vbtable[2]);
    IHero* hero = owner_hero(unit);
    return hero && hero->has_artifact(ARTIFACT_GOLDEN_GOOSE) ? multiplier + golden_goose_bonus : multiplier;
}

// Runs the original function as a subroutine (replayed first two instructions, then its own ret comes back here),
// then adjusts the multiplier in st0.
__declspec(naked) void GoldenGooseFork() {
    __asm
    {
        push ecx
        call GOLDEN_GOOSE_ORIGINAL          // st0 = good luck multiplier
        pop ecx
        sub esp, 0x4
        fstp dword ptr [esp]
        call golden_goose_multiplier        // st0 = adjusted multiplier, pops the float
        ret

    GOLDEN_GOOSE_ORIGINAL:
        mov eax, dword ptr [ecx + 0x4]      // replicate overwritten game code
        mov edx, dword ptr [eax + 0x8]
        jmp [GoldenGoose_return]
    }
}
