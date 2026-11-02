#include "pch.h"
#include "Artfset.h"

// EDIT MAGIC PROOF FROM ARTIFACTS
// (13) Breastplate of the Forgotten Hero : 15% proof
// (174) Ancient Protection Charm : 10% proof
// (117) Genji's Vest : 20% proof
// Dwarven set (17), 6 pieces : 50% proof (ArtfsetDwarven.cpp)

void SpellProofFork();

int SpellProof_fork = 0x0097AFC0;
int SpellProof_return = 0x0097AFC7;

void SpellProof_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, SpellProof_fork, 7, SpellProofFork, 0, 0, 0 });
}

// Each artifact the defender wears multiplies the damage factor and shows its buff on the notified unit
static void proof_artifact(IHero* defender, int artifact, float factor, float* damage_factor, int* notified) {
    if (!defender->has_artifact(artifact)) return;
    notify_artifact_buff(notified, 0, artifact, 0, 0);
    *damage_factor *= factor;
}

static void __fastcall spell_proof(IHero* defender, ICombatUnit* target, float* damage_factor, int* notified) {
    proof_artifact(defender, ARTIFACT_SENTINELS_BREASTPLATE, constf_0_85, damage_factor, notified);
    proof_artifact(defender, ARTIFACT_ANCIENT_PROTECTION_CHARM, constf_0_9, damage_factor, notified);
    proof_artifact(defender, ARTIFACT_GENJIS_VEST, constf_0_8, damage_factor, notified);
    // artifact sets, for spells with a target unit
    if (target && artfset_dwarven_spell_proof(defender))
        *damage_factor *= 0.5f;
}

// ebx = defending IHero (not null), edi = target unit (may be 0), ebp = spell, [esp+0x48] = damage factor,
// [esp+0x4C] = unit notified of the artifact buffs
__declspec(naked) void SpellProofFork() {
    __asm
    {
        push dword ptr [esp + 0x4C]     // notified
        lea eax, [esp + 0x4C]
        push eax                        // damage factor ([esp+0x48] before the push)
        mov edx, edi
        mov ecx, ebx
        call spell_proof

        mov ecx, ebp
        call [get_spell_school]
        jmp [SpellProof_return]
    }
}
