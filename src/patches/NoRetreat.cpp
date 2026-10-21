#include "pch.h"

// (7) No Retreat : all creatures in the combat (friendly and enemy) have 0 speed when either hero carries it.
// They cannot move, but can still be teleported.
// Hooks the end of func_CombatCreature_ComputeSpeedModifiers (0x008A1E50, CombatUnit +0x48), the creature combat speed.
// The vanilla effect is kept: neither side can retreat or surrender (artifact 7 checks at 0x004D9F34 / 0x004D9F66 in
// the retreat check 0x004D9E20, reason 4).

void NoRetreatFork();

int NoRetreat_fork = 0x008A20DF;    // xor eax,eax / test esi,esi / setl al (final speed in esi, clamped to >= 0)
int NoRetreat_return = 0x008A20E6;  // pop edi / sub eax,1 / and eax,esi

void NoRetreat_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, NoRetreat_fork, 7, NoRetreatFork, 0, 0, 0 });
}

static bool has_no_retreat(ICombatHero* hero) {
    return hero && hero->has_artifact(ARTIFACT_NO_RETREAT);
}

// unit = creature CombatUnit subobject (vbptr at this-0x144)
static int __fastcall no_retreat_speed(ICombatCreature* unit, int speed) {
    return has_no_retreat(unit->owner()) || has_no_retreat(unit->opponent()) ? 0 : speed;
}

// ebp = creature CombatUnit subobject, esi = final speed
__declspec(naked) void NoRetreatFork() {
    __asm
    {
        mov ecx, ebp
        mov edx, esi
        call no_retreat_speed
        mov esi, eax

        xor eax, eax                        // replicate overwritten game code
        test esi, esi
        setl al
        jmp [NoRetreat_return]
    }
}
