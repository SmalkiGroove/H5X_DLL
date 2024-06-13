#include "pch.h"
//# Fixes
//# - Empowered Armageddon not benefiting from "Master of Fire" and "Ignite" perks
//# - Empowered Armageddon stone hitting central squares did not do additional damage as the spell specify
//# - (ToE) Empowered Armageddon not hitting War Machines

void EmpoweredArmageddon_check_holly_unholly_spells();
void EmpoweredArmageddon_WarMachines();

void EmpoweredArmageddon_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_CALL, 0x0085F6DD, 6, EmpoweredArmageddon_check_holly_unholly_spells, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_WRTE, 0x0085F6E3, 1, nullptr, 0, 0, 0, "74" }); // change JNE to JE
    assembly_patches.push_back({ PATCH_WRTE, 0x0085F4EE, 2, nullptr, 0, 0, 0, "9090" }); // fixes armageddon bonus damage not applied at center
    assembly_patches.push_back({ PATCH_HOOK, 0x0085F58A, 6, EmpoweredArmageddon_WarMachines, 0, 0, 0, 0 });
}

__declspec(naked) void EmpoweredArmageddon_check_holly_unholly_spells() {
    __asm
    {
        mov eax, dword ptr [ebx + 0x4]
        cmp eax, 0x15
        je JUMP_IF_UNHOLLY_WORD
        cmp eax, 0x23
        JUMP_IF_UNHOLLY_WORD :
        ret
    }
}

int EmpoweredArmageddon_hit_the_target = 0x0085F59F;
int EmpoweredArmageddon_return = 0x0085F590;
__declspec(naked) void EmpoweredArmageddon_WarMachines() {
    __asm
    {
        mov eax, dword ptr [ebx + 0x4]
        cmp eax, 0xA
        je HIT_THE_TARGET
        sub eax, 0xE8
        test al, al
        je HIT_THE_TARGET
        jmp [EmpoweredArmageddon_return]
    HIT_THE_TARGET:
        jmp [EmpoweredArmageddon_hit_the_target]
    }
}