#include "pch.h"

// (105) Naga Commander's Helmet : +1 diplomacy level when neutral creatures decide to join, flee or fight
// (stacks with the Diplomacy perk: 0 -> 1, 1 -> 2; the hero +0x310 bonus is a head artifact too, so 2 is the max).
// Patches the neutral creature join check (0x008E1AD0): the level is 0 without PERK_DIPLOMACY, else 1 or 2, and
// FUN_008da210 lowers the creature mood modifier (0.5 / 1.0 / 1.5 / 2.0 by mood) per level.
// Diplomacy level: -0.25 mood modifier per level instead of -0.2 (all heroes, not only with the helmet).

void NagaCommandersHelmetFork();

int NagaCommandersHelmet_fork = 0x008E1C2A;     // after get_skill_mastery(PERK_DIPLOMACY): xor ecx,ecx / test eax,eax /
                                                // setle cl / sub ecx,1 / and ecx,esi / mov [esp+1Ch],ecx
int NagaCommandersHelmet_return = 0x008E1C3A;
int DiplomacyLevelFactor_operand = 0x008DA247;  // FUN_008da210: fmul dword ptr [0x00E12FF8] (shared 0.2 constant)

void NagaCommandersHelmet_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, NagaCommandersHelmet_fork, 16, NagaCommandersHelmetFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_FLOAT_PTR, DiplomacyLevelFactor_operand, 4, nullptr, 0, constf_0_25, 0 });
}

// obj = the join check's hero holder (vtable +0x14 returns the IHero), level = diplomacy level of the game
static int __fastcall naga_helmet_diplomacy_level(int* obj, int level) {
    IHero* hero = (IHero*)((GetPtr)(*(int***)obj)[0x14 / 4])(obj);
    return hero && hero->has_artifact(ARTIFACT_NAGA_COMMANDERS_HELMET) ? level + 1 : level;
}

// eax = Diplomacy mastery, esi = level when the hero has Diplomacy (1, or 2 with the +0x310 bonus), ebx = hero holder
__declspec(naked) void NagaCommandersHelmetFork() {
    __asm
    {
        xor ecx, ecx                            // replicate overwritten game code: ecx = mastery > 0 ? esi : 0
        test eax, eax
        setle cl
        sub ecx, 1
        and ecx, esi

        mov edx, ecx
        mov ecx, ebx
        call naga_helmet_diplomacy_level
        mov ecx, eax
        mov dword ptr [esp + 0x1C], ecx
        jmp [NagaCommandersHelmet_return]
    }
}
