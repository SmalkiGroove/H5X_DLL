#include "pch.h"

// 4: scripted
// 4 Haven: scripted
// 6: creatures always get good morale

void ArtfsetHaven4Fork();

int ArtfsetHaven4_fork = 0x004E66C2;
int ArtfsetHaven4_return = 0x004E66C7;
int ArtfsetHaven4_end = 0x004E6730;

void ArtfsetHaven4_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ArtfsetHaven4_fork, 5, ArtfsetHaven4Fork, 0, 0, 0 });
}

__declspec(naked) void ArtfsetHaven4Fork() {
    __asm
    {
        
        jmp[ArtfsetHaven4_return]
    }
}
