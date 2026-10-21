#include "pch.h"
#include "Artfset.h"

// EDIT SPELL PENETRATION FROM ARTIFACTS
// Damaging spells cast by the hero ignore part of the target's magic resistance (resisted part multiplied, like
// Arcanism right after; the multipliers stack):
// (119) Genji's Silksword : 20% spell penetration
// (137) Chaos Shroud : 66% spell penetration

// SPELLS NEVER RESISTED (no resistance roll, no spell proof / resistance multiplier; immunities still apply)
// Inferno set (13), 4 pieces + Inferno hero : fire spells of the hero's side against enemy units (ArtfsetInferno.cpp)
// Frost set (21), 4 pieces : ice spells of the hero's side against enemy units (ArtfsetFrost.cpp)

void SpellPenetrationFork();
void ResistanceBypassFork();

int ResistanceBypass_fork = 0x0097D2FB;    // func_ComputeSpellDamageMultiplier, after the hit chance: cmp eax,64h / mov [esp+14h],eax
int ResistanceBypass_return = 0x0097D302;

// func_ComputeSpellDamageMultiplier, damaging spell of a hero caster (edi, +0x74 checked non-null):
// has_skill(SKILL_ARCANISM) check replaced; resisted part (float) at [esp+0x10]
int SpellPenetration_fork = 0x0097D43B;
int SpellPenetration_return = 0x0097D464;   // Arcanism: fild table[mastery] (eax = mastery)
int SpellPenetration_end = 0x0097D47F;      // no Arcanism

void SpellPenetration_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpellPenetration_fork, 41, SpellPenetrationFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, ResistanceBypass_fork, 7, ResistanceBypassFork, 0, 0, 0 });
}

// spell = spell ID (get_spell_id), caster / target = CombatUnit subobjects (either may be 0)
static int __fastcall resistance_bypassed(int spell, ICombatUnit* caster, ICombatUnit* target) {
    if (!caster || !target) return 0;
    return artfset_inferno_ignores_resistance(spell, caster, target)
        || artfset_frost_ignores_resistance(spell, caster, target);
}

// func_ComputeSpellDamageMultiplier (__fastcall, ret 18h) returns the % of the spell resisted (0..100).
// eax = hit chance (100 = immune), ebp = spell ID, edi = caster unit, [esp+0x34] = target unit (1st stack arg).
// Not immune and bypassed: return 0 (nothing resisted) before the resistance roll and func_ComputeSpellDamageMitigation.
__declspec(naked) void ResistanceBypassFork() {
    __asm
    {
        mov dword ptr [esp + 0x14], eax
        cmp eax, 0x64
        je RESISTANCE_VANILLA
        push dword ptr [esp + 0x34]
        mov edx, edi
        mov ecx, ebp
        call resistance_bypassed
        test eax, eax
        jne RESISTANCE_BYPASSED
        mov eax, dword ptr [esp + 0x14]
        cmp eax, 0x64

    RESISTANCE_VANILLA:
        jmp [ResistanceBypass_return]

    RESISTANCE_BYPASSED:
        xor eax, eax
        pop edi
        pop esi
        pop ebp
        pop ebx
        add esp, 0x20
        ret 0x18
    }
}

// caster = hero CombatUnit subobject; returns the multiplier of the resisted part
static float __fastcall artifact_penetration_multiplier(int* caster) {
    int* aux = ((GetPtr)(*(int***)caster)[0x74 / 4])(caster);       // get_hero_aux_subobject
    IHero* hero = (IHero*)((GetPtr)(*(int***)aux)[0])(aux);
    float multiplier = 1.0f;
    if (hero->has_artifact(ARTIFACT_GENJIS_SILKSWORD)) multiplier *= 0.8f;
    if (hero->has_artifact(ARTIFACT_CHAOS_SHROUD))     multiplier *= 0.34f;
    return multiplier;
}

__declspec(naked) void SpellPenetrationFork() {
    __asm
    {
        mov ecx, edi
        call artifact_penetration_multiplier
        fmul dword ptr ss: [esp + 0x10]
        fstp dword ptr ss: [esp + 0x10]

        // Arcanism mastery of the hero

        mov edx, dword ptr [edi]
        mov ecx, edi
        call dword ptr [edx + 0x74]
        mov edx, dword ptr [eax]
        mov ecx, eax
        call dword ptr [edx]
        mov edx, dword ptr [eax]
        mov ecx, eax
        push 0x12
        call dword ptr [edx + 0x174]
		test eax, eax
        je SPELL_PENETRATION_END
        jmp[SpellPenetration_return]

        SPELL_PENETRATION_END:
        jmp[SpellPenetration_end]
    }
}
