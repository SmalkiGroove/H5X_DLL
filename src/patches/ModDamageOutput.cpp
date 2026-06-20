#include "pch.h"

// EDIT DAMAGE OUTPUT MODIFIERS FROM ARTIFACTS
// (132) Butcher Glaive : +15% melee damage
// (129) Avenger Bow : +10% ranged damage
// (190) Celestial Justicar Armor : -25% melee damage
// (122) Sentinel's Shield : -10% ranged damage
// (133) Celestial Justicar Shield : -50% ranged damage
// (198) Shield of Dwarven Kings : -30% ranged damage
// Skill Courage reduce all damages per Morale point
// Skill Arcane Protection reduce all damages by 0.5% per spellpower

void MeleeDamageInputFork();
void RangedDamageInputFork();
void MeleeDamageOutputFork();
void RangedDamageOutputFork();
void AllDamageOutputFork();

int MeleeDamageInput_fork = 0x00A58E74;
int MeleeDamageInput_return = 0x00A58E79;
int RangedDamageInput_fork = 0x00A58EBD;
int RangedDamageInput_return = 0x00A58EC3;
int MeleeDamageOutput_fork = 0x00A5AC59;
int MeleeDamageOutput_return = 0x00A5AC5F;
int RangedDamageOutput_fork = 0x00A5ACF9;
int RangedDamageOutput_return = 0x00A5ACFF;
int AllDamageOutput_fork = 0x00A5AD6E;
int AllDamageOutput_return = 0x00A5AD75;

void DamageOutput_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, MeleeDamageInput_fork, 5, MeleeDamageInputFork, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, RangedDamageInput_fork, 6, RangedDamageInputFork, 0, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, MeleeDamageOutput_fork, 6, MeleeDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, RangedDamageOutput_fork, 6, RangedDamageOutputFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, AllDamageOutput_fork, 7, AllDamageOutputFork, 0, 0, 0 });
}

__declspec(naked) void MeleeDamageInputFork() {
    __asm
    {
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x84
        call[count_equipped_artifact]
        test eax, eax
        je MELEE_INPUT_END
        fld dword ptr ss: [esp + 0x38]
        fmul dword ptr [constf_1_15]
        fstp dword ptr ss: [esp + 0x38]

        MELEE_INPUT_END :
        mov edx, dword ptr [ebp]
        mov ecx, ebp
        jmp[MeleeDamageInput_return]
    }
}

__declspec(naked) void RangedDamageInputFork() {
    __asm
    {
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x81
        call[count_equipped_artifact]
        test eax, eax
        je RANGED_INPUT_END
        fld dword ptr ss: [esp + 0x38]
        fmul dword ptr [constf_1_1]
        fstp dword ptr ss: [esp + 0x38]

        RANGED_INPUT_END:
        mov eax, dword ptr [ebp + 0x4]
        mov ecx, dword ptr [eax + 0x8]
        jmp[RangedDamageInput_return]
    }
}

__declspec(naked) void MeleeDamageOutputFork() {
    __asm
    {
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xBE
        call[count_equipped_artifact]
        test eax, eax
        je MELEE_OUTPUT_END
        fld dword ptr ss : [esp + 0x24]
        fmul dword ptr [constf_0_75]
        fstp dword ptr ss : [esp + 0x24]

    MELEE_OUTPUT_END:
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        jmp[MeleeDamageOutput_return]
    }
}

__declspec(naked) void RangedDamageOutputFork() {
    __asm
    {
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x7A
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_2
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_9]
        fstp dword ptr ss : [esp + 0x18]        

    RANGE_OUTPUT_2:
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0x85
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_3
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_5]
        fstp dword ptr ss : [esp + 0x18]

    RANGE_OUTPUT_3:
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov ecx, eax
        push 0xC6
        call[count_equipped_artifact]
        test eax, eax
        je RANGE_OUTPUT_END
        fld dword ptr ss : [esp + 0x18]
        fmul dword ptr [constf_0_7]
        fstp dword ptr ss : [esp + 0x18]

    RANGE_OUTPUT_END:
        mov edx, dword ptr [edi + 0x4]
        mov eax, dword ptr [edx + 0x8]
        jmp[RangedDamageOutput_return]
    }
}

__declspec(naked) void AllDamageOutputFork() {
    __asm
    {
        test edi, edi
        jz ALL_DAMAGE_OUTPUT_RETURN
        mov eax, dword ptr [edi]
        mov ecx, edi
        call dword ptr [eax]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0x4
        call dword ptr [edx + 0x174]
        test eax, eax
        jz ALL_DAMAGE_OUTPUT_ARCANE_PROTECTION
        mov ecx, dword ptr ss : [esp + 0x20]
        mov dword ptr ss : [esp + 0x20], eax
        fild dword ptr ss : [esp + 0x20]
        fmul dword ptr [constf_percent]
        mov dword ptr ss : [esp + 0x20], ecx
        mov eax, dword ptr [ebp]
        mov ecx, ebp
        call dword ptr [eax + 0x1B0]
        mov ecx, dword ptr ss : [esp + 0x20]
        mov dword ptr ss : [esp + 0x20], eax
        fimul dword ptr ss : [esp + 0x20]
        fsubr dword ptr [constf_1]
        fmul dword ptr ss : [esp + 0x18]
        fstp dword ptr ss : [esp + 0x18]
        mov dword ptr ss : [esp + 0x20], ecx

        ALL_DAMAGE_OUTPUT_ARCANE_PROTECTION:
        mov edx, dword ptr [edi + 0x4]
		mov eax, dword ptr [edx + 0x8]
        mov edx, dword ptr [eax + edi + 0x4]
        lea ecx, dword ptr [eax + edi + 0x4]
        push 0x36
        call dword ptr [edx + 0x290]
        test al, al
        jz ALL_DAMAGE_OUTPUT_RETURN

        // Total spellpower (base + skills + artifacts + combat buffs):
        // CombatHero vtable +0x244 = GetEffectiveSpellPower(spellRef*, int flag), ret 8.
        // spellRef = { spell* (may be 0), spellId (0 = no spell context), 0, 0, 0 }
        xor eax, eax
        push eax
        push eax
        push eax
        push eax
        push eax                            // zeroed 5-dword spellRef on the stack
        mov edx, dword ptr [edi + 0x4]
        mov eax, dword ptr [edx + 0x8]
        mov edx, dword ptr [eax + edi + 0x4]
        lea ecx, dword ptr [eax + edi + 0x4]
        push 0                              // arg2 = flag (always 0 at game call sites)
        lea eax, dword ptr [esp + 0x4]
        push eax                            // arg1 = &spellRef
        call dword ptr [edx + 0x244]        // eax = total spellpower, callee pops both args
        add esp, 0x14                       // free spellRef

        sar eax, 0x1
        test eax, eax
        jz ALL_DAMAGE_OUTPUT_RETURN
        push eax
        fild dword ptr ss: [esp]
        fmul dword ptr[constf_percent]
        fsubr dword ptr [constf_1]
        fmul dword ptr ss: [esp + 0x1C]     // damage is at [esp+0x18] before the push
        fstp dword ptr ss: [esp + 0x1C]
        add esp, 0x4

        ALL_DAMAGE_OUTPUT_RETURN:
        mov edx, dword ptr [ebx]
        mov ecx, ebx
        call dword ptr [edx + 0xC]

        jmp[AllDamageOutput_return]
    }
}
