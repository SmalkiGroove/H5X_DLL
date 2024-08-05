#include "pch.h"

// EDIT MAGIC PROOF FROM ARTIFACTS
// (13) Breastplate of the Forgotten Hero : 10% MR
// (27) Boots of Interference : 15% MR
// (174) Ancient Protection Charm : 12% MR
// (184) Forgotten Ring : 8% MR

void MagicProofFork();

void MagicProof_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, 0x0097CB1F, 5, MagicProofFork, 0, 0, 0 });
}

__declspec(naked) void MagicProofFork() {
    __asm
    {

    }
}
