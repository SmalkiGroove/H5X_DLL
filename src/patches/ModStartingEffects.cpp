#include "pch.h"
#include "types/Artifacts.h"
#include "types/Spells.h"
#include "Artfset.h"

// EFFECTS PUT ON EVERY CREATURE AT COMBAT START BY THE ENEMY HERO (neutrals included)
// Frost set (21), 4 pieces : Frozen (ArtfsetFrost.cpp)
// Spirit set (26), 5 pieces : Sorrow for the whole combat (ArtfsetSpirit.cpp)
// (159) Shackles of the Last Man : effect SPELL_EFFECT_SHACKLES_OF_THE_LAST_MAN on the enemy creatures, display only
//       (the -2 attack / defense are ModAttack.cpp / ModDefense.cpp)
//
// Effects with an H5X spell ID (353 and above) need the spell table expansion of the exe
// (heroes-expansion/game_binary/patches/h5toe/h5m_spells-count.yml): they are skipped while func_GetSpellCount
// does not cover them.

void StartingEffectsFork();

// func_InitCombatEffects (creature combat start, esi = CCombatCreature), after the initial ATB and Agility, on every
// creature (the owner hero test comes after)
int StartingEffects_fork = 0x008AB267;    // mov eax,[esi+4] / mov ecx,[eax+8]
int StartingEffects_return = 0x008AB26D;

static const int combat_start_time = 10000;   // time of the combat start effects (rage, ATB queue)
static const int permanent_effect_value = 0;  // value stored with the effect (effect +0x20 / +0x40)
typedef int(__cdecl* SpellCountFn)();
static const SpellCountFn spell_count = (SpellCountFn)0x00847890;  // func_GetSpellCount: size of the spell table

void StartingEffects_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, StartingEffects_fork, 6, StartingEffectsFork, 0, 0, 0 });
}

// Display-only effect for the whole combat. The engine gives no behaviour to an H5X effect ID: it is only an icon /
// tooltip. Skipped while the spell table does not cover it.
static void add_display_effect(ICombatUnit* unit, int effect, ICombatUnit* caster) {
	if (effect >= spell_count()) return;
	unit->add_permanent_effect(effect, permanent_effect_value, 0, 0, combat_start_time, caster);
}

// creature = CCombatCreature (CombatUnit at +4 + [[+4]+8])
static void __fastcall starting_effects(char* creature) {
	ICombatUnit* unit = (ICombatUnit*)(creature + 4 + *(int*)(*(int*)(creature + 4) + 8));
	// enemy side (vtable +0x10) -> its NWorld::ICombatHero (vtable +0x0C)
	int* enemy_side = ((ThiscallIntPtr_IntPtr)unit->instance->notify_owner)((int*)unit);
	ICombatHero* enemy = combat_creature_detail::combat_hero_from_outer(combat_creature_detail::call_unit_ref(enemy_side));
	if (!enemy) return;
	IHero* hero = enemy->hero();
	if (!hero) return;

	artfset_frost_starting_freeze(unit, enemy, combat_start_time);
	artfset_spirit_starting_sorrow(unit, enemy, combat_start_time);
	if (hero->has_artifact(ARTIFACT_SHACKLES_OF_THE_LAST_MAN))
		add_display_effect(unit, SPELL_EFFECT_SHACKLES_OF_THE_LAST_MAN, enemy);
}

__declspec(naked) void StartingEffectsFork() {
	__asm
	{
		mov ecx, esi
		call starting_effects

		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		jmp [StartingEffects_return]
	}
}
