#include "pch.h"

// ADD SPELL IMMUNITIES FROM ADDITIONAL ARTIFACTS
// (184) Forgotten Ring : Immune to Word of Light (35)
// (180) Moonlight Band : Immune to Curse of the Netherworld (21)
// (123) Obsidian Armor : Immune to Armageddon (10)
// (172) Sanctuary's Crown : Immune to Hypnotize (20)
// (115) Cloak of the Saint : Immune to Plague (14)
// (165) Shantiri Breastplate : Immune to Earthquake (41)

void SpellImmuneFork();

int Addr_fork_immunity = 0x0097CE1E;
int Addr_return_immunity = 0x0097CE23;
int Addr_end_immunity = 0x0097CE92;
int Addr_apply_immunity = 0x0097CE7E;

void SpellImmune_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_BYTE, 0x0097CC2D, 1, nullptr, 184, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, Addr_fork_immunity, 7, SpellImmuneFork, 0, 0, 0 });
}

__declspec(naked) void SpellImmuneFork() {
    __asm
    {
        mov ecx, eax
        cmp ebx, 0xA
        jne SPELL_IMMUNITY_1
        push 0x7B
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_TO_END
        push 0
        push 3
        push 0x7B
        jmp[Addr_apply_immunity]

        SPELL_IMMUNITY_1:
        cmp ebx, 0x14
        jne SPELL_IMMUNITY_2
        push 0xAC
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_TO_END
        push 0
        push 3
        push 0xAC
        jmp[Addr_apply_immunity]

        SPELL_IMMUNITY_2:
        cmp ebx, 0x15
        jne SPELL_IMMUNITY_3
        push 0xB4
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_TO_END
        push 0
        push 3
        push 0xB4
        jmp[Addr_apply_immunity]

        SPELL_IMMUNITY_3:
        cmp ebx, 0x29
        jne SPELL_IMMUNITY_4
        push 0xA5
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_TO_END
        push 0
        push 3
        push 0xA5
        jmp[Addr_apply_immunity]

        SPELL_IMMUNITY_4:
        cmp ebx, 0xE
        jne RETURN_FROM_FORK
        push 0x73
        call[count_equipped_artifact]
        test eax, eax
        je SKIP_TO_END
        push 0
        push 3
        push 0x73
        jmp[Addr_apply_immunity]

        RETURN_FROM_FORK:
        jmp[Addr_return_immunity]

        SKIP_TO_END:
        jmp[Addr_end_immunity]
    }
}

/*
ESI = spell id
EDI = creature
*/