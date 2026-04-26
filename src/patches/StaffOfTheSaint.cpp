#include "pch.h"

// STAFF OF THE SAINT GRANTS +7 EFFECTIVE SPELLPOWER FOR LIGHT MAGIC SPELLS

void StaffOfTheSaintFork();

int StaffOfTheSaint_fork = 0x00BC0473;
int StaffOfTheSaint_return = 0x00BC0479;

void StaffOfTheSaint_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, StaffOfTheSaint_fork, 6, StaffOfTheSaintFork, 0, 0, 0 });
}

const float StaffOfTheSaint_spellpowerBonus = 7.0f;
__declspec(naked) void StaffOfTheSaintFork() {
    __asm
    {
        lea ecx, dword ptr [esi + 0xA0]
        mov eax, dword ptr [ecx]
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x86
        call[count_equipped_artifact]
        test eax, eax
        je STAFF_OF_THE_SAINT_RETURN
        add ebx, 0x7
        
        STAFF_OF_THE_SAINT_RETURN:
        mov eax, dword ptr [esi + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        jmp[StaffOfTheSaint_return]
    }
}
