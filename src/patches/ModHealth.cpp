#include "pch.h"
#include "Artfset.h"

// ADD BONUS HP FROM ADDITIONAL ARTIFACTS
// (128) Plunderer Vest : +1 HP
// (169) Ring of Supernatural Strength : +3 HP
// (156) Barbarian Cuirass : +3% HP
// (100) Regal Armour : +6% HP (rounded up)
// Dwarven set (17), 6 pieces : +5 HP (ArtfsetDwarven.cpp)

void HealthFork();

int Health_fork = 0x004BC4F4;    // creature HP with hero bonuses, after the Ring of Vitality count: mov ecx,[ebx+1Ch] / lea ebp,[edi+eax*2]
int Health_return = 0x004BC4FA;  // call 0x0089FBF0 (creature data, ecx = creature id)

void Health_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, Health_fork, 6, HealthFork, 0, 0, 0 });
}

// Flat HP added by the hero's artifacts to each creature of the stack.
static int __fastcall health_artifact_bonus(IHero* hero, ICreature* creature) {
    int bonus = 0;
    if (hero->has_artifact(ARTIFACT_PLUNDERER_VEST))  bonus += 1;
    if (hero->has_artifact(ARTIFACT_RING_OF_STRENGTH)) bonus += 3;
    bonus += artfset_dwarven_health(hero);

    int base_health = get_creature_data(creature->type_id)[0x64 / 4];
    if (hero->has_artifact(ARTIFACT_BARBARIAN_CUIRASS)) bonus += base_health / 32;      // ~3%
    if (hero->has_artifact(ARTIFACT_REGAL_ARMOUR))      bonus += base_health / 16 + 1;  // ~6%, rounded up

    return bonus;
}

// esi = IHero, ebx = ICreature, edi = HP bonus so far, eax = Ring of Vitality count; result in ebp
__declspec(naked) void HealthFork() {
    __asm
    {
        lea ebp, dword ptr [edi + eax * 2]    // vanilla Ring of Vitality: +2 per ring

        mov ecx, esi
        mov edx, ebx
        call health_artifact_bonus
        add ebp, eax

        mov ecx, dword ptr [ebx + 0x1C]
        jmp [Health_return]
    }
}
