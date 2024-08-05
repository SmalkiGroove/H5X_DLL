#include "pch.h"

// EDIT MAGIC RESISTANCE FROM ARTIFACTS
// (13) Breastplate of the Forgotten Hero : 10% MR
// (27) Boots of Interference : 15% MR
// (174) Ancient Protection Charm : 12% MR
// (184) Forgotten Ring : 8% MR

void MagicResistFork();

void MagicResist_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, 0x0097CB1F, 5, MagicResistFork, 0, 0, 0 });
}

__declspec(naked) void MagicResistFork() {
    __asm
    {

    }
}
