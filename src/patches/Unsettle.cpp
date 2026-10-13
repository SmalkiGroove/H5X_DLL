#include "pch.h"
#include "types/Spells.h"

// PERK_UNSETTLE: earth damage spells of the hero remove 1 max HP from each creature of the damaged stack
// (stack HP reduction += 1 / creature max HP, applied before the damage).

void UnsettleSpellDamageFork();

int UnsettleTargeted_fork = 0x0097DF67;   // targeted spell damage: call apply_spell_damage
int UnsettleArea_fork = 0x0085ECE2;       // area spell damage: call apply_spell_damage

void Unsettle_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_CALL, UnsettleTargeted_fork, 5, UnsettleSpellDamageFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, UnsettleArea_fork, 5, UnsettleSpellDamageFork, 0, 0, 0 });
}

static void __fastcall unsettle(ICombatUnit* caster, ICombatUnit* target, int spell, int time) {
	if (!caster || !target || !spell) return;
	if (caster->unit_kind() != 2 || target->unit_kind() != 1) return;  // hero caster, creature target
	if (!((ICombatHero*)caster)->has_perk(PERK_UNSETTLE)) return;
	if (get_spell_element((int*)spell) != ELEMENT_EARTH) return;

	ICombatStack* stack = ((ICombatCreature*)target)->stack();
	if (!stack) return;
	int max_hp = stack->creature_health();  // before any reduction
	if (max_hp <= 1) return;
	stack->add_health_reduction(time, 1.0f / max_hp);
}

// Replaces both calls to apply_spell_damage (0x004E6B60, __fastcall):
// ecx = caster, edx = target, stack args = arg3, time, spell ID, arg6.
// Applies Unsettle, then jumps to apply_spell_damage with the same registers, arguments and return address.
__declspec(naked) void UnsettleSpellDamageFork() {
	__asm {
		push ecx
		push edx
		push dword ptr [esp + 0x10]     // time    ([esp+0x8] at entry)
		push dword ptr [esp + 0x18]     // spell   ([esp+0xC] at entry)
		call unsettle                   // ecx = caster, edx = target
		pop edx
		pop ecx
		jmp [apply_spell_damage]
	}
}
