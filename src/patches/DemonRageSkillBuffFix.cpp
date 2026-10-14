#include "pch.h"
#include "types/Spells.h"

// FIX: description of the rage effects (SPELL_DEMON_RAGE_LEVEL_1..3) given by a skill's spellBuffs (HERO_SKILL_TRAINING)
// The rage effect description (func_BuildDemonRageDescription, from the effect's description getter func_GetEffectDescriptionParams) lists the
// DefaultStats DemonRage.Bonuses of the creature ID stored in the effect value (+0x20). The real rage stores the
// creature ID there, but the skill buffs (func_ApplySkillSpellBuffs, CombatUnit +0x260 with duration 0x7FFFFFFD) store the skill
// mastery: at Expert (3) every creature showed the bonuses of creature 3 (Landlord, same as the Peasant).
// The bonuses themselves only test the presence of the effect (func_GetDemonRageLevel) and use the stack's own creature.
// For these 3 effects the value becomes the creature ID of the unit.

void DemonRageSkillBuffFork();

int DemonRageSkillBuff_fork = 0x00C55CCD;    // func_ApplySkillSpellBuffs: push edi (mastery) / push 7FFFFFFDh, then push ebx (effect)
int DemonRageSkillBuff_return = 0x00C55CD3;

void DemonRageSkillBuffFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, DemonRageSkillBuff_fork, 6, DemonRageSkillBuffFork, 0, 0, 0 });
}

// unit = CombatUnit receiving the skill buff, effect = spell ID of the buff
static int __fastcall skill_buff_value(ICombatUnit* unit, int effect, int mastery) {
	if (effect < SPELL_DEMON_RAGE_LEVEL_1 || effect > SPELL_DEMON_RAGE_LEVEL_3) return mastery;
	ICreature* creature = ((ICombatCreature*)unit)->creature();
	return creature ? creature->type_id : mastery;
}

// esi = unit, ebx = effect, edi = mastery; edx must hold the unit's vtable for the call [edx+260h]
__declspec(naked) void DemonRageSkillBuffFork() {
	__asm
	{
		push edi
		mov edx, ebx
		mov ecx, esi
		call skill_buff_value
		push eax
		push 0x7FFFFFFD
		mov edx, dword ptr [esi]
		jmp [DemonRageSkillBuff_return]
	}
}
