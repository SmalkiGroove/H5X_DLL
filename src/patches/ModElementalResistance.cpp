#include "pch.h"

// EDIT ELEMENTAL RESISTANCE ARTIFACTS
// (9) Iceberg Shield : from +50% to +40% to fire resistance
// (20) Magnetic Ring : from +50% to +33% to lightning resistance
// (43) Dragon Flame Tongue : from +50% to +40% to ice resistance
// (62) Cloak of Sylanna : from +50% to +33% to earth resistance
// (84) Frozen Heart : from +25% to +33% to ice resistance
// (85) Phoenix Feather Cape : from +25% to +33% to fire resistance
// (124) Black Hole : +67% to all element resistance

void ElemProofFork();
void FireProofAbilityFork();

int ElemProof_fork = 0x0097B0C5;           // func_ComputeSpellDamageMitigation, element switch: ja 0x0097B23F (eax = element - 1)
int ElemProof_return = 0x0097B0CB;         // jmp [eax*4+0x0097B400]
int ElemProof_end = 0x0097B23F;            // after the switch

int FireProofAbility_fork = 0x0097B172;    // fire case, target fire-proof ability (0xAD): jmp 0x0097B231
int FireProofAbility_return = 0x0097B23B;  // fstp [esp+48h]

float elemproof = 0.67f;

void ElemProof_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B0FF, 4, nullptr, 0, constf_0_6, 0, 0 }); // 9
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B237, 4, nullptr, 0, elemproof, 0, 0 }); // 20,62
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B1D3, 4, nullptr, 0, constf_0_6, 0, 0 }); // 43
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B204, 4, nullptr, 0, elemproof, 0, 0 }); // 84
    assembly_patches.push_back({ PATCH_FLOAT_PTR, 0x0097B130, 4, nullptr, 0, elemproof, 0, 0 }); // 85
    assembly_patches.push_back({ PATCH_HOOK, ElemProof_fork, 6, ElemProofFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, FireProofAbility_fork, 5, FireProofAbilityFork, 0, 0, 0 });
}

// resisted = the damage fraction the function accumulates ([esp+0x48]); each factor below 1 lowers the damage.

// Applied before the element cases, for element spells only.
// defender = hero of the target's side (may be 0), target = target unit (may be 0).
static void __fastcall elemental_resistance_bonuses(IHero* defender, ICombatUnit* target, int spell, int* notify_context, float* resisted) {
    if (!defender) return;

    // Black Hole: +60% resistance to every element
    if (defender->has_artifact(ARTIFACT_BLACK_HOLE)) {
        notify_artifact_buff(notify_context, 0, ARTIFACT_BLACK_HOLE, 0, 0);
        *resisted *= 0.33f;
    }

    // Magic Filter: +80% resistance to one element per unit, derived from the unit pointer
    if (target && defender->skill_mastery(PERK_MAGIC_FILTER) != 0) {
        int filtered_element = (((unsigned)target >> 8) & 0x3) + 1;
        if (filtered_element == get_spell_element((int*)spell))
            *resisted *= 0.2f;
    }
}

// Fire-proof ability: keeps the vanilla x0.5 (the shared multiplier at 0x0097B235 is patched to 0.67 above).
static void __fastcall fire_proof_ability(float* resisted) {
    *resisted *= 0.5f;
}

// ebx = defender IHero (may be 0), edi = target unit (may be 0), ebp = spell, esi = notify context,
// [esp+0x48] = resisted fraction. Flags from cmp eax,3 (element - 1).
__declspec(naked) void ElemProofFork() {
    __asm
    {
        ja JUMPTABLE_DEFAULT_CASE

        lea eax, dword ptr [esp + 0x48]
        push eax                        // resisted
        push esi                        // notify_context
        push ebp                        // spell
        mov edx, edi
        mov ecx, ebx
        call elemental_resistance_bonuses

        mov ecx, ebp                    // the jump table needs element - 1 in eax again
        call [get_spell_element]
        add eax, -1
        jmp [ElemProof_return]

    JUMPTABLE_DEFAULT_CASE:
        jmp [ElemProof_end]
    }
}

__declspec(naked) void FireProofAbilityFork() {
    __asm
    {
        lea ecx, dword ptr [esp + 0x48]
        call fire_proof_ability
        jmp [FireProofAbility_return]
    }
}
