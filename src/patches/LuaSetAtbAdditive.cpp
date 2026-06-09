#include "pch.h"

// Make combat script setATB(unit, value) add value to the current ATB instead of overwriting it.
//
// lua_setATB (0x0056CF60) posts a CSetATBValue command (vtable 0x00E18184) to the arena:
//   cmd+0xC = unit, cmd+0x10 = float value.
// The command execute method at 0x004E0970 calls unit->SetATB(value) through vtable +0x18C.
// Vtable +0x188 returns the unit's ATB info object, whose float at +0x1C is the current ATB
// (same pointer the SetATB implementation at 0x00A6B690 dereferences).
// We replace the execute tail to call SetATB(current + value) instead.

void LuaSetAtbAdditive();

int LuaSetAtbAdditive_fork = 0x004E098B;

void LuaSetAtbAdditive_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, LuaSetAtbAdditive_fork, 20, LuaSetAtbAdditive, 0, 0, 0, 0 });
}

__declspec(naked) void LuaSetAtbAdditive() {
    __asm
    {
        // ecx = CSetATBValue command object (unit already checked alive)
        push esi
        mov esi, ecx
        mov eax, dword ptr [esi + 0xC]      // unit
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x188]        // eax = ATB info object
        fld dword ptr [eax + 0x1C]          // current ATB
        fadd dword ptr [esi + 0x10]         // + script value
        push eax
        fstp dword ptr [esp]                // float argument = current + value
        mov eax, dword ptr [esi + 0xC]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x18C]        // unit->SetATB(sum), callee pops arg
        pop esi
        mov al, 1
        ret
    }
}
