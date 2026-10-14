#include "pch.h"
#include "types/Spells.h"

// SPEC_LIGHTNING_TOUCH: the hero's creatures deal 10% bonus damage as lightning on melee attacks (resisted like Lightning Bolt).
// The bonus is added to the attack damage like the fire / cold bonuses, and shown with the Stormstrike bolt
// (spell visual 0xF7 "STORMSTRIKE", the ABILITY_STORMSTRIKE visual).

void LightningTouchPreviewMinFork();
void LightningTouchPreviewMaxFork();
void LightningTouchAttackFork();

// func_ComputeAttackPrediction, attack prediction (cursor hint): min / max damage, before func_GetAdditionalFireDmg (estimate = 1).
// Ranged flag: byte [esp+0x1C] in the body (can shoot (CombatUnit +0x1E0) and not forced to melee), [esp+0x24] at both hooks.
int LightningTouchPreviewMin_fork = 0x00A634FD;    // push edi / push 0 / mov edx,ebx
int LightningTouchPreviewMin_return = 0x00A63502;  // mov ecx,esi / call func_GetAdditionalFireDmg
int LightningTouchPreviewMax_fork = 0x00A63554;    // push ebp / push 0 / mov edx,ebx
int LightningTouchPreviewMax_return = 0x00A63559;
// func_ResolveAttackHit, attack resolution: after func_GetAdditionalWaterDmg
int LightningTouchAttack_fork = 0x00A67C7C;        // mov ecx,[ebp] / mov edx,[ecx]
int LightningTouchAttack_return = 0x00A67C81;

const int lightning_touch_spell = SPELL_LIGHTNING_BOLT;      // element of the bonus for the target's resistance
const int lightning_touch_visual = SPELL_ABILITY_STORMBOLT;  // 0xF7: the visual launcher plays the "STORMSTRIKE" bolt
const float lightning_touch_ratio = 0.1f;

void LightningTouch_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, LightningTouchPreviewMin_fork, 5, LightningTouchPreviewMinFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LightningTouchPreviewMax_fork, 5, LightningTouchPreviewMaxFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, LightningTouchAttack_fork, 5, LightningTouchAttackFork, 0, 0, 0 });
}

// Hero of the attacker's side: attacker -> side -> NWorld::ICombatHero (same chain as func_GetAdditionalFireDmg).
static bool has_lightning_touch(ICombatUnit* attacker) {
    using namespace combat_creature_detail;
    ICombatHero* hero = combat_hero_from_outer(call_unit_ref(attacker->unit_ref()));
    return hero && hero->has_perk(SPEC_LIGHTNING_TOUCH);
}

// Lightning damage added to an attack dealing `damage` (0 without the perk).
static int lightning_touch_damage(ICombatUnit* attacker, ICombatUnit* target, int damage, int estimate) {
    if (damage <= 0 || !has_lightning_touch(attacker)) return 0;
    return compute_magic_damage(lightning_touch_spell, nullptr, damage * lightning_touch_ratio, (int*)target, 0, estimate, 1, 0);
}

// Stormstrike bolt from the attacker to the target at `time` (same call as ABILITY_STORMSTRIKE, 0x00A60964).
// Like the fire / cold bonus events, nothing is shown when the combat flag CCombat +0x98 (vtable +0x108) is set.
static void lightning_touch_show(ICombatUnit* attacker, ICombatUnit* target, int time) {
    int* combat = ((GetPtr)attacker->instance->get_owner_ref)((int*)attacker);
    if (!combat) return;
    typedef char(__thiscall* CombatFlagFn)(int*);
    if (((CombatFlagFn)(*(int***)combat)[0x108 / 4])(combat)) return;

    int spell_ref[5] = { 0, lightning_touch_visual, 0, 0, 0 };
    launch_spell_visual(spell_ref, (int*)attacker, (int*)target, time, 0x00F495D4);
    destroy_spell_ref(spell_ref);
}

static int __fastcall lightning_touch_estimate(ICombatUnit* attacker, ICombatUnit* target, int damage, int ranged) {
    if (ranged) return 0;
    return lightning_touch_damage(attacker, target, damage, 1);
}

// context = attack context (func_ResolveAttackHit this): +0x00 attacker, +0x14 ranged (byte), +0x1C estimate, +0x38 time.
// bonus_damage = elemental bonus summed with the fire and cold bonuses (0x00A67D7E).
static void __fastcall lightning_touch_attack(int* context, ICombatUnit* target, int damage, int* bonus_damage) {
    if (((char*)context)[0x14]) return;  // ranged attack
    ICombatUnit* attacker = (ICombatUnit*)context[0];
    int estimate = context[0x1C / 4];
    int lightning = lightning_touch_damage(attacker, target, damage, estimate);
    if (lightning <= 0) return;
    *bonus_damage += lightning;
    if (!estimate) lightning_touch_show(attacker, target, context[0x38 / 4]);
}

// esi = attacker, ebx = target, edi = min damage. The fire bonus keeps the damage without lightning, as in the attack.
__declspec(naked) void LightningTouchPreviewMinFork() {
    __asm
    {
        movzx eax, byte ptr [esp + 0x24]
        push eax                        // ranged
        push edi
        mov edx, ebx
        mov ecx, esi
        call lightning_touch_estimate

        push edi
        push 0
        add edi, eax
        mov edx, ebx
        jmp [LightningTouchPreviewMin_return]
    }
}

// esi = attacker, ebx = target, ebp = max damage
__declspec(naked) void LightningTouchPreviewMaxFork() {
    __asm
    {
        movzx eax, byte ptr [esp + 0x24]
        push eax                        // ranged
        push ebp
        mov edx, ebx
        mov ecx, esi
        call lightning_touch_estimate

        push ebp
        push 0
        add ebp, eax
        mov edx, ebx
        jmp [LightningTouchPreviewMax_return]
    }
}

// ebp = attack context, ebx = target, esi = damage (passed to the fire / cold bonuses), eax = cold bonus,
// [esp+0x64] = elemental bonus
__declspec(naked) void LightningTouchAttackFork() {
    __asm
    {
        push eax
        lea eax, dword ptr [esp + 0x68]
        push eax                        // bonus_damage
        push esi                        // damage
        mov edx, ebx
        mov ecx, ebp
        call lightning_touch_attack
        pop eax

        mov ecx, dword ptr [ebp]
        mov edx, dword ptr [ecx]
        jmp [LightningTouchAttack_return]
    }
}
