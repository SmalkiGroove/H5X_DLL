#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "types/Spells.h"
#include "Artfset.h"

// DUNGEON ARTIFACT SET (16)
// 4: hero's effective spellpower is doubled for dark magic spells (applied by ModEffectiveSpellpower.cpp)
// 4 Dungeon: the hero has the perk SPEC_DARKNESS_AFFINITY (47): Slow and Confusion become SPELL_MASS_SLOW and
//   SPELL_MASS_FORGETFULNESS in the spellbook. Hook below: the hero's skill mastery reports the perk as learnt.
// 6: hero's creatures are immune to Curse of the Netherworld (UnholyWordImmunitiesFix.cpp) and get +2 initiative
//   (ModInitiative.cpp)
// 6 Dungeon: hero's dark elves (base creature 71, 73, 77, 81 and their upgrades) are immune to mind control
//   (shared has_ability hook, ModAbilities.cpp)

void DungeonDarknessAffinityFork();

int DungeonDarknessAffinity_fork = 0x00B4D0B0;    // IHero skill mastery (vtable +0x174, ret 4): cmp dword ptr [esp+4],0 / push ebp
int DungeonDarknessAffinity_return = 0x00B4D0B6;  // mov ebp,ecx / jg (flags of the cmp)

static const float dungeon_initiative = 2.0f;
static const int dungeon_dark_elves[] = { 71, 73, 77, 81 };  // base creatures

void ArtfsetDungeon_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, DungeonDarknessAffinity_fork, 6, DungeonDarknessAffinityFork, 0, 0, 0 });
}

static int dungeon_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_DUNGEON) : 0;
}

static bool is_dungeon_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_DUNGEON_MIGHT || hero_class == HERO_CLASS_DUNGEON_MAGIC;
}

bool artfset_dungeon_spellpower_doubled(IHero* hero, int* spell) {
	return get_spell_school(spell) == SPELL_SCHOOL_DARK && dungeon_pieces(hero) >= 4;
}

// Called by the skill mastery hook for SPEC_DARKNESS_AFFINITY (no skill mastery call inside: no recursion)
static int __fastcall dungeon_darkness_affinity(IHero* hero) {
	return dungeon_pieces(hero) >= 4 && is_dungeon_class(hero);
}

bool artfset_dungeon_unholy_word_immune(ICombatUnit* unit) {
	return dungeon_pieces(owner_hero(unit)) >= 6;
}

float artfset_dungeon_initiative(IHero* hero) {
	return dungeon_pieces(hero) >= 6 ? dungeon_initiative : 0.0f;
}

static bool is_dark_elf(ICombatUnit* unit) {
	ICreature* creature = ((ICombatCreature*)unit)->creature();
	if (!creature) return false;
	int base = get_creature_data(creature->type_id)[0x100 / 4];  // base creature of an upgrade, 0 for a base creature
	if (base == 0) base = creature->type_id;
	for (int id : dungeon_dark_elves)
		if (base == id) return true;
	return false;
}

// Ability 19, through the shared has_ability hook (ModAbilities.cpp, after Sandro's Cloak)
int artfset_dungeon_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_IMMUNITY_TO_MIND_CONTROL) return 0;
	IHero* hero = owner_hero(unit);
	if (dungeon_pieces(hero) < 6 || !is_dungeon_class(hero)) return 0;
	return is_dark_elf(unit);
}

// ecx = IHero, [esp+4] = skill. The perk counts as learnt (mastery 1); the spellbook refresh (0x00B53B10, run by
// func_CheckArtifactPassiveEffects / func_GainNewPerk / func_RemoveHeroSkill) then adds or removes the mass spells.
__declspec(naked) void DungeonDarknessAffinityFork() {
	__asm
	{
		cmp dword ptr [esp + 0x4], SPEC_DARKNESS_AFFINITY
		jne DARKNESS_AFFINITY_VANILLA
		push ecx
		call dungeon_darkness_affinity
		pop ecx
		test eax, eax
		jz DARKNESS_AFFINITY_VANILLA
		mov eax, 1
		ret 4

	DARKNESS_AFFINITY_VANILLA:
		cmp dword ptr [esp + 0x4], 0
		push ebp
		jmp [DungeonDarknessAffinity_return]
	}
}
