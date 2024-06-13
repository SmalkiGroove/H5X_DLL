#include "pch.h"
// Change: Elite Casters affect only spells from the 4 spell schools. Previously it wrongly boosted abilities.

void EliteCasters();

void EliteCasters_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, 0x00978397, 5, EliteCasters, 0, 0, 0, 0 });
}

int EliteCasters_goto_cost_reduction = 0x0097839C;
int EliteCasters_skip_cost_reduction = 0x009783CD;
__declspec(naked) void EliteCasters() {
    __asm {
        cmp dword ptr [esp + 0x18] , 0x3
        jg NOT_A_SPELL
        push 0x94
        jmp[EliteCasters_goto_cost_reduction]
        NOT_A_SPELL:
        jmp[EliteCasters_skip_cost_reduction]
    }
}
