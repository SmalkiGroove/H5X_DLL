#include "pch.h"

// (57) Mercenary Band : the hero's creatures minimum damage is raised to the average of their minimum and maximum
// damage (rounded down), e.g. 6-10 -> 8-10.
// Hooks the entry of func_GetCreatureMinDamage (0x004BBFB0, thiscall(IHero* hero, int), ret 8): runs it, then reads
// the maximum from func_GetCreatureMaxDamage (0x004BC020, same signature, includes BloodyClawNecklace.cpp).

void MercenaryBandFork();

int MercenaryBand_fork = 0x004BBFB0;     // push esi / mov esi,[esp+8]
int MercenaryBand_return = 0x004BBFB5;
static ThiscallInt_IntPtr_IntPtr_Int get_creature_max_damage = (ThiscallInt_IntPtr_IntPtr_Int)0x004BC020;

void MercenaryBand_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, MercenaryBand_fork, 5, MercenaryBandFork, 0, 0, 0 });
}

// self = `this` of the getters, hero = their IHero argument (may be null), arg = their second argument
static int __fastcall calc_min_damage(int* self, int min_damage, IHero* hero, int arg) {
    if (!hero) return min_damage;
    // same "object being destroyed" check as the getters, which then ignore the hero
    char* hero_bytes = (char*)hero;
    int vb_offset = (*(int**)(hero_bytes + 4))[1];
    if (*(unsigned char*)(hero_bytes + vb_offset + 0xB) & 0x80) return min_damage;
    if (!hero->has_artifact(ARTIFACT_MERCENARY_BAND)) return min_damage;

    int average = (min_damage + get_creature_max_damage(self, (int*)hero, arg)) / 2;
    return average > min_damage ? average : min_damage;
}

// Entered by jmp from the getter entry: ecx = this, [esp] = return address, [esp+4] = hero, [esp+8] = arg
__declspec(naked) void MercenaryBandFork() {
    __asm
    {
        push ecx
        push dword ptr [esp + 0xC]          // arg
        push dword ptr [esp + 0xC]          // hero
        call BOOTS_ORIGINAL                 // eax = min damage
        pop ecx                             // this

        push dword ptr [esp + 0x8]          // arg
        push dword ptr [esp + 0x8]          // hero
        mov edx, eax
        call calc_min_damage                // pops hero / arg
        ret 0x8

    BOOTS_ORIGINAL:
        push esi                            // replicate overwritten game code
        mov esi, dword ptr [esp + 0x8]
        jmp [MercenaryBand_return]
    }
}
