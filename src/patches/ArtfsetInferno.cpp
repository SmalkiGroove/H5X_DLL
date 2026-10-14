#include "pch.h"
#include "types/Abilities.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"
#include "types/Spells.h"
#include "Artfset.h"

// INFERNO ARTIFACT SET (13)
// 4: hero's creatures gain Liquid Flame Breath (ability 168). The game only spawns the flames on the tiles of a
//    pattern attack (breath...), so a melee attack without pattern gets the target's tile closest to the attacker.
// Liquid Flame Breath damage (any creature): DefaultStats +0x3AC per creature, the stack counting as
//    count / weekly growth (rounded, >= 1) instead of count, so a tier 7 (growth 1) is unchanged.
// 4 Inferno: fire spells cast by the hero's side are never resisted by enemy units: no resistance roll, no spell
//            proof / resistance multiplier at all (abilities, buffs, artifacts, skills). Immunities still apply.
//            (resistance bypass hook in ModSpellPenetration.cpp)
// 6: enemy units get -5 luck
// 6 Inferno: hero's creatures gain Fire Shield (ability 62)

void ArtfsetInfernoEnemyLuckFork();
void ArtfsetInfernoFlameTilesFork();
static int __fastcall liquid_flame_count(ICombatUnit* unit);

static const int inferno6_luck_malus = 5;

int ArtfsetInfernoEnemyLuck_fork = 0x00989E70;    // func_ComputeLuckModifiers, after the enemy hero debuffs: mov eax,[esp+1Ch] / test eax,eax
int ArtfsetInfernoEnemyLuck_return = 0x00989E76;
int ArtfsetInfernoFlameTiles_fork = 0x00A629DA;   // func_ApplyAfterHitEffects, attacker has ability 168: mov ecx,[esi+40h] / cmp ecx,[esi+44h]
int ArtfsetInfernoFlameTiles_return = 0x00A629E0;
int ArtfsetInfernoFlameCountNew_fork = 0x0098B13F;  // liquid flame creation: call [edx+1D8h] (count), then mov edi,eax
int ArtfsetInfernoFlameCountCmp_fork = 0x0098AD5D;  // damage of a would-be flame, compared to an existing one: same code

void ArtfsetInferno_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetInfernoEnemyLuck_fork, 6, ArtfsetInfernoEnemyLuckFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetInfernoFlameTiles_fork, 6, ArtfsetInfernoFlameTilesFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, ArtfsetInfernoFlameCountNew_fork, 6, (void*)liquid_flame_count, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, ArtfsetInfernoFlameCountCmp_fork, 6, (void*)liquid_flame_count, 0, 0, 0 });
}

static int inferno_pieces(IHero* hero) {
	return hero ? hero->artifact_set_count(ARTIFACT_SET_INFERNO) : 0;
}

static bool is_inferno_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_INFERNO_MIGHT || hero_class == HERO_CLASS_INFERNO_MAGIC;
}

// Called for every ability query that reaches the creature's own ability list.
// Abilities 62 / 168, through the shared has_ability hook (ModAbilities.cpp)
int artfset_inferno_ability(ICombatUnit* unit, int ability) {
	if (ability != ABILITY_LIQUID_FLAME_BREATH && ability != ABILITY_FIRE_SHIELD) return 0;
	IHero* hero = owner_hero(unit);
	int pieces = inferno_pieces(hero);
	if (ability == ABILITY_LIQUID_FLAME_BREATH) return pieces >= 4;
	return pieces >= 6 && is_inferno_class(hero);
}

bool artfset_inferno_ignores_resistance(int spell, ICombatUnit* caster, ICombatUnit* target) {
	if (get_spell_element((int*)spell) != ELEMENT_FIRE) return false;
	IHero* hero = owner_hero(caster);
	if (inferno_pieces(hero) < 4 || !is_inferno_class(hero)) return false;
	return owner_hero(target) != hero;  // enemy units only (neutrals have no hero)
}

// enemy_hero = the combat hero facing the unit whose luck is computed
static int __fastcall inferno6_enemy_luck(int* enemy_hero, int luck) {
	IHero* hero = combat_creature_detail::combat_hero_from_outer(enemy_hero)->hero();
	if (inferno_pieces(hero) >= 6) luck -= inferno6_luck_malus;
	return luck;
}

// Melee attack of a creature with Liquid Flame Breath and no pattern tiles (attack +0x40 empty): when the ability
// comes from the set, add the target's (attack +0x04) tile closest to the attacker so that the flame is spawned there.
static void __fastcall inferno4_flame_tiles(int* attack) {
	ICombatUnit* attacker = (ICombatUnit*)attack[0];
	ICombatUnit* target = (ICombatUnit*)attack[1];
	if (!attacker || !target) return;
	if (inferno_pieces(owner_hero(attacker)) < 4) return;

	// all target tiles, appended by the game (engine allocator), {int x, int y} each
	typedef void(__thiscall* GetOccupiedTilesFn)(int*, int*, int);
	int* tiles = attack + 0x40 / 4;  // first, last, end
	((GetOccupiedTilesFn)target->instance->get_occupied_tiles)((int*)target, tiles, 0);
	int* first = (int*)tiles[0];
	int count = ((int*)tiles[1] - first) / 2;
	if (count <= 1) return;

	// attacker center: its position is the max corner of its size x size tiles
	typedef void(__thiscall* GetPositionFn)(int*, int*);
	int position[2];
	((GetPositionFn)attacker->instance->get_position_2d)((int*)attacker, position);
	float offset = (attacker->instance->get_size((int*)attacker) - 1) * 0.5f;
	float center_x = position[0] - offset;
	float center_y = position[1] - offset;

	int best = 0;
	float best_distance = 0.0f;
	for (int i = 0; i < count; i++) {
		float dx = first[2 * i] - center_x;
		float dy = first[2 * i + 1] - center_y;
		float distance = dx * dx + dy * dy;
		if (i == 0 || distance < best_distance) {
			best = i;
			best_distance = distance;
		}
	}
	first[0] = first[2 * best];
	first[1] = first[2 * best + 1];
	tiles[1] = (int)(first + 2);  // keep one tile (capacity unchanged)
}

// Liquid Flame Breath: number of creatures the flame damage is multiplied by (vanilla: current count).
static int __fastcall liquid_flame_count(ICombatUnit* unit) {
	int scaled = (int)(((ICombatCreature*)unit)->growth_ratio() + 0.5f);
	return scaled < 1 ? 1 : scaled;
}

// edi = enemy combat hero (or 0), esi = luck modifier being returned
__declspec(naked) void ArtfsetInfernoEnemyLuckFork() {
	__asm
	{
		test edi, edi
		je INFERNO_LUCK_RETURN
		mov ecx, edi
		mov edx, esi
		call inferno6_enemy_luck
		mov esi, eax

	INFERNO_LUCK_RETURN:
		mov eax, dword ptr [esp + 0x1C]
		test eax, eax
		jmp [ArtfsetInfernoEnemyLuck_return]
	}
}

// esi = attack (FUN_00A62890 layout: +0x00 attacker, +0x04 target, +0x40/+0x44/+0x48 vector of {x, y} tiles)
__declspec(naked) void ArtfsetInfernoFlameTilesFork() {
	__asm
	{
		mov ecx, dword ptr [esi + 0x40]
		cmp ecx, dword ptr [esi + 0x44]
		jne INFERNO_FLAME_TILES_RETURN
		mov ecx, esi
		call inferno4_flame_tiles
		mov ecx, dword ptr [esi + 0x40]
		cmp ecx, dword ptr [esi + 0x44]

	INFERNO_FLAME_TILES_RETURN:
		jmp [ArtfsetInfernoFlameTiles_return]
	}
}
