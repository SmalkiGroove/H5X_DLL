#include "pch.h"
#include "types/Spells.h"
#include "structs/Spell.h"

// EARTHQUAKE (41) IS A DAMAGE SPELL AGAINST CREATURES
// Vanilla: siege only, it damages walls and towers; the Earthquake perk (skill 139) also deals physical damage to every
// creature (DefaultStats +0x66C + spellpower x +0x670, no resistance) and removes DefaultStats +0x674 ATB.
// H5X:
// - castable in every combat (the cast check no longer requires a wall);
// - walls and towers: unchanged, still from the <damage> table;
// - creatures: spell damage from the <duration> table (Base + PerPower x spellpower of the caster's mastery) through the
//   regular damage spell pipeline, like Magic Arrow or Armageddon: <Element> / <DamageIsElemental> (elemental damage,
//   element immunities), magic immunity / resistance, spell proof, artifact immunities (ModSpellImmune.cpp) and the
//   other spell damage modifiers. Creatures that take damage and survive still lose the perk's ATB;
// - only the creatures allowed by <Target>: TARGET_HOSTILE enemies, TARGET_FRIEND allies, TARGET_NEUTRAL all;
// - flying creatures (CombatUnit +0x190, so Plumed Boots too) are immune.

void EarthquakeBaseDamageFork();
void EarthquakeImmunityFork();
void EarthquakeCreaturesFork();

// func_CheckSpellCastable (0x0097C320), Earthquake case: refuses the cast when func_CollectEarthquakeTargets finds no wall
int EarthquakeCastable_fork = 0x0097C38C;          // jne 0x0097C423 (cast allowed) -> jmp
// func_IsDamagingSpell (0x004E5CB0): byte table 0x004E5D24 (spell ID - 1) -> jump table 0x004E5D18 (0 = true)
int EarthquakeDamaging_entry = 0x004E5D4C;         // SPELL_EARTHQUAKE: 2 (false) -> 0 (true)
// func_PrepareSpellDamageParams (0x00976B90, base damage of a damage spell): after func_GetSpellID
int EarthquakeBaseDamage_fork = 0x00976BA6;        // mov esi,eax / cmp esi,0DEh
int EarthquakeBaseDamage_return = 0x00976BAE;      // jg (flags of the cmp)
int EarthquakeBaseDamage_clamp = 0x00976BEF;       // fcom 1.0: the <damage> table path after its evaluation
int EarthquakeDurationTable = 0x00A34CD0;          // func_GetSpellDurationTableValue: <duration> table (ecx = spell, edx = mastery, spellpower)
// func_ComputeSpellHitChance (0x0097C9F0, ret 0Ch): eax = target unit, [esp+4] = spell ID, [esp+8] = caster.
// Returns the resisted percentage, 100 = immune.
int EarthquakeImmunity_fork = 0x0097C9F0;          // sub esp,18h / push ebx / mov ebx,[esp+24h]
int EarthquakeImmunity_return = 0x0097C9F8;
// func_CastSpell, case 0x29 (0x0097E228), after the walls: the perk block (has_skill(139), then func_CollectAllCombatUnits,
// func_IsValidSpellSplashTarget, physical damage, ATB loss, CSpellSplashEvent) is replaced.
int EarthquakeCreatures_fork = 0x0097E513;         // mov eax,[esi] / push 8Bh
int EarthquakeCreatures_end = 0x0097E718;          // end of the perk block (frees the wall list)

static const int earthquake_atb_loss = 0x674;      // DefaultStats float: ATB lost by the creatures hit

void Earthquake_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, EarthquakeCastable_fork, 6, nullptr, 0, 0, 0, "E99200000090" });
	assembly_patches.push_back({ PATCH_BYTE, EarthquakeDamaging_entry, 1, nullptr, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, EarthquakeBaseDamage_fork, 8, EarthquakeBaseDamageFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, EarthquakeImmunity_fork, 8, EarthquakeImmunityFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, EarthquakeCreatures_fork, 7, EarthquakeCreaturesFork, 0, 0, 0 });
}

static bool __fastcall earthquake_immune(ICombatUnit* unit) {
	return unit->combat_stack() && unit->is_flying();
}

static int spell_target(int spell) {
	SSpell* record = (SSpell*)get_spell_record(spell);
	return record ? record->target : SPELL_TARGET_HOSTILE;
}

static bool earthquake_can_hit(ICombatUnit* caster, ICombatUnit* unit, int target) {
	if (!unit->combat_stack() || unit->is_dead() || earthquake_immune(unit)) return false;
	if (target == SPELL_TARGET_NEUTRAL) return true;
	bool ally = is_ally_of_caster((int*)caster, (int*)unit) != 0;
	return target == SPELL_TARGET_FRIEND ? ally : !ally;
}

// frame = func_CastSpell's esp at the hook: +0x44 cast context copy (spell handle, spell ID, ...), +0x70 spellpower,
// +0x74 mastery, +0x78 damage multiplier (float), +0xC4 func_CastSpell's first stack argument.
// Every creature is hit at `time`, like the perk; returns the end of the last damage.
static int __stdcall earthquake_hit_creatures(char* frame, int* combat, ICombatUnit* caster, int time) {
	int* spell_ref = (int*)(frame + 0x44);
	int spell = spell_ref[1];
	int spellpower = *(int*)(frame + 0x70);
	int mastery = *(int*)(frame + 0x74);
	float multiplier = *(float*)(frame + 0x78);
	int cast_arg = *(int*)(frame + 0xC4);
	int target = spell_target(spell);
	int* week = ((GetPtr)(*(int***)combat)[0x3C / 4])(combat);  // as [esp+0x20] for the other damage spells
	float atb_loss = *(float*)((char*)get_defaultstats() + earthquake_atb_loss);

	int units[3] = { 0, 0, 0 };
	collect_all_combat_units(combat, units);
	int end = time;
	for (char** entry = (char**)units[0]; entry != (char**)units[1]; ++entry) {
		char* object = *entry;
		if (!object) continue;
		ICombatUnit* unit = (ICombatUnit*)(object + 4 + *(int*)(*(int*)(object + 4) + 8));
		if (!earthquake_can_hit(caster, unit, target)) continue;

		int damage = prepare_spell_damage(spell, spellpower, mastery, week, nullptr);
		damage = get_spell_damage(damage, spell_ref, (int*)caster, (int*)unit, 1, time, multiplier, 1);
		damage += compute_elemental_spell_damage(time, damage, (int*)caster, (int*)unit, SPELL_EARTHQUAKE, 0);
		if (damage <= 0) continue;

		int hit_end = apply_spell_damage((int*)caster, (int*)unit, damage, time, SPELL_EARTHQUAKE, cast_arg);
		if (hit_end > end) end = hit_end;
		if (!unit->is_dead()) unit->set_atb(unit->atb() - atb_loss);
	}
	if (units[0]) engine_free((void*)units[0]);
	return end;
}

// esi = spell ID, edi = spellpower, [esp+0x14] = mastery. Earthquake evaluates <duration> instead of <damage>.
__declspec(naked) void EarthquakeBaseDamageFork() {
	__asm
	{
		mov esi, eax
		cmp esi, SPELL_EARTHQUAKE
		je EARTHQUAKE_DURATION
		cmp esi, 0xDE
		jmp [EarthquakeBaseDamage_return]

	EARTHQUAKE_DURATION:
		mov edx, dword ptr [esp + 0x14]
		push edi
		mov ecx, esi
		call [EarthquakeDurationTable]
		jmp [EarthquakeBaseDamage_clamp]
	}
}

__declspec(naked) void EarthquakeImmunityFork() {
	__asm
	{
		cmp dword ptr [esp + 0x4], SPELL_EARTHQUAKE
		jne EARTHQUAKE_IMMUNITY_VANILLA
		test eax, eax
		jz EARTHQUAKE_IMMUNITY_VANILLA
		push eax
		mov ecx, eax
		call earthquake_immune
		test al, al
		pop eax
		jz EARTHQUAKE_IMMUNITY_VANILLA
		mov eax, 100
		ret 0xC

	EARTHQUAKE_IMMUNITY_VANILLA:
		sub esp, 0x18
		push ebx
		mov ebx, dword ptr [esp + 0x24]
		jmp [EarthquakeImmunity_return]
	}
}

// esi = caster, ebx = combat ([esp+0x24]), ebp = time (func_CastSpell's return value)
__declspec(naked) void EarthquakeCreaturesFork() {
	__asm
	{
		mov eax, esp
		push ebp
		push esi
		push ebx
		push eax
		call earthquake_hit_creatures
		mov ebp, eax
		jmp [EarthquakeCreatures_end]
	}
}
