#include "pch.h"

// INCREASE BALLISTA ATTACK BY 10

void RingOfDevastationFork();

int RingOfDevastation_fork = 0x004BC6A6;
int RingOfDevastation_return = 0x004BC6AC;

void RingOfDevastation_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, RingOfDevastation_fork, 6, RingOfDevastationFork, 0, 0, 0 });
}

static int __fastcall ring_of_devastation_attack(IHero* hero, int attack) {
    return hero->has_artifact(ARTIFACT_RING_OF_DEVASTATION) ? attack + 10 : attack;
}

// esi = IHero, edi = ballista attack
__declspec(naked) void RingOfDevastationFork() {
    __asm
    {
        mov edx, edi
        mov ecx, esi
        call ring_of_devastation_attack
        mov edi, eax

        mov eax, dword ptr [esi]
        mov ecx, esi
        push 0x8
        jmp [RingOfDevastation_return]
    }
}
