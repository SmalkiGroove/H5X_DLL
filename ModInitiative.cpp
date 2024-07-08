#include "pch.h"

// ADD IITIATIVE FROM ADDITIONAL ARTIFACTS
// 

void InitiativeFork();

void Initiative_init(pugi::xml_document& doc) {
    //assembly_patches.push_back({ PATCH_HOOK, 0x0097CB1F, 5, InitiativeFork, 0, 0, 0 });
}

__declspec(naked) void InitiativeFork() {
    __asm
    {

    }
}
