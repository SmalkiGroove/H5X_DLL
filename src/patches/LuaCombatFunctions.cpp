#include "pch.h"
#include "LuaScript.h"
#include "structs/Combat.h"

// LUA: H5X COMBAT SCRIPT FUNCTIONS
// The combat context gets an extra function table: the engine builds its context registry (0x00837790, from the CRT
// start-up, before the DLL runs) as a list of table pointers per context name. The combat context's pointer to the
// one-entry cinematic table (0x00F33080, EnableCinematicCamera) is replaced by H5X_COMBAT_FUNCTIONS, which starts with
// that entry. The tutorial context keeps the vanilla table.
// Ports of the advmap hero functions, same arguments, results and errors as the advmap handlers. The hero is one of
// the combat heroes, given by its script name or by its combat unit name ("attacker-hero" / "defender-hero"):
// - GetHeroLevel(hero)                                   (0x005C3190: IHero vtable +0x20)
// - GetHeroStat(hero, stat)                              (0x005C33A0: stat 0..9, member function table 0x00FD9100)
// - GetHeroSkillMastery(hero, skill)                     (0x005C4140: IHero vtable +0x174)
// - HasArtefact(hero, artifact, equipped_only = 0)       (0x005C4CD0: equipped 0x004BA880, else backpack 0x005C06F0)
// - GetArtifactSetItemsCount(hero, set, equipped = 1)    (0x005C57C0: IHero vtable +0x32C sets map, 0x00B09FD0)
// - KnowHeroSpell(hero, spell)                           (0x005C4770: IHero vtable +0xD0)
// Also random(a, b) (LuaRandom.cpp).
// Hero lookup: CCombat (arena vtable +0x08) vtable +0x120 = script names, its vtable +0x04(name) = unit, unit vtable
// +0x74 = ICombatHero, its vtable +0x00 = IHero (as lua_GetHeroName 0x00568290); IHero vtable +0x38 = script name.

int __fastcall lua_combat_GetHeroLevel(int* state);
int __fastcall lua_combat_GetHeroStat(int* state);
int __fastcall lua_combat_GetHeroSkillMastery(int* state);
int __fastcall lua_combat_HasArtefact(int* state);
int __fastcall lua_combat_GetArtifactSetItemsCount(int* state);
int __fastcall lua_combat_KnowHeroSpell(int* state);

static const LuaFunction H5X_COMBAT_FUNCTIONS[] = {
	{ "EnableCinematicCamera", (void*)0x0056E370 },   // the vanilla table this one replaces
	{ "random", lua_random },
	{ "GetHeroLevel", lua_combat_GetHeroLevel },
	{ "GetHeroStat", lua_combat_GetHeroStat },
	{ "GetHeroSkillMastery", lua_combat_GetHeroSkillMastery },
	{ "HasArtefact", lua_combat_HasArtefact },
	{ "GetArtifactSetItemsCount", lua_combat_GetArtifactSetItemsCount },
	{ "KnowHeroSpell", lua_combat_KnowHeroSpell },
	{ nullptr, nullptr }
};

static const int lua_cinematic_table = 0x00F33080;

void LuaCombatFunctions_init(pugi::xml_document& doc) {
	lua_replace_context_table("combat", lua_cinematic_table, H5X_COMBAT_FUNCTIONS);
}

typedef char(__thiscall* HasItemFn)(int*, int);
typedef int(__thiscall* SetCountFn)(int*, int, int);
typedef int(__fastcall* CountFn)();
typedef char(__fastcall* IsSchoolSpellFn)(int);

// IHero of a combat hero unit, nullptr if the unit doesn't exist or isn't a hero.
static IHero* unit_hero(ICombat* combat, const char* unit_name) {
	size_t length = strlen(unit_name);
	const char* name[3] = { unit_name, unit_name + length, unit_name + length };   // engine string {begin, end, capacity}
	ICombatUnit* unit = (ICombatUnit*)combat->unit_names()->find_unit(name);
	if (!unit) return nullptr;
	ICombatHeroInterface* combat_hero = (ICombatHeroInterface*)unit->hero_interface();
	if (!combat_hero || engine_object_released((int*)combat_hero)) return nullptr;
	return combat_hero->hero();
}

static bool hero_has_name(IHero* hero, const char* name) {
	const char** script_name = hero->script_name();
	size_t length = strlen(name);
	return (size_t)(script_name[1] - script_name[0]) == length && memcmp(script_name[0], name, length) == 0;
}

// Argument 1: hero script name or combat hero unit name. Reports the error and returns nullptr on failure.
static IHero* combat_hero_arg(int* state, const char* function) {
	const char* name = lua_string_arg(state, 1);
	if (!name || !*name) {
		lua_error(state, "%s: hero name is not specified", function);
		return nullptr;
	}
	IArena* arena = (IArena*)lua_owner(state, RTTI_COMBAT_ARENA);
	if (!arena) {
		lua_error(state, "%s: can't get combat arena", function);
		return nullptr;
	}
	ICombat* combat = arena->combat();
	IHero* hero = unit_hero(combat, name);
	if (hero) return hero;
	static const char* const hero_units[] = { "attacker-hero", "defender-hero" };
	for (const char* unit_name : hero_units) {
		hero = unit_hero(combat, unit_name);
		if (hero && hero_has_name(hero, name)) return hero;
	}
	lua_error(state, "%s: hero \"%s\" is not in this combat", function, name);
	return nullptr;
}

int __fastcall lua_combat_GetHeroLevel(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "GetHeroLevel");
	if (!hero) return 0;
	lua_push_number(state, hero->level());
	return 1;
}

// 0x00FD9100: 10 pointers to IHero member functions, 12 bytes each: function, this adjustment, offset in the vbtable
struct HeroStatGetter {
	int function;
	int adjustment;
	int vbtable_offset;
};
static const HeroStatGetter* const hero_stat_getters = (const HeroStatGetter*)0x00FD9100;

int __fastcall lua_combat_GetHeroStat(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "GetHeroStat");
	if (!hero) return 0;
	int stat;
	if (!lua_number_arg(state, 2, &stat) || stat < 0 || stat > 9) {
		lua_error(state, "GetHeroStat: incorrect stat id");
		return 0;
	}
	const HeroStatGetter& getter = hero_stat_getters[stat];
	char* vbtable = *(char**)((char*)hero + 4);
	int* self = (int*)((char*)hero + 4 + *(int*)(vbtable + getter.vbtable_offset) + getter.adjustment);
	lua_push_number(state, ((ThiscallInt_IntPtr)getter.function)(self));
	return 1;
}

int __fastcall lua_combat_GetHeroSkillMastery(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "GetHeroSkillMastery");
	if (!hero) return 0;
	int skill;
	if (!lua_number_arg(state, 2, &skill) || skill < 0 || skill >= ((CountFn)0x008478B0)()) {
		lua_error(state, "GetHeroSkillMastery: invalid skill id");
		return 0;
	}
	lua_push_number(state, hero->skill_mastery(skill));
	return 1;
}

int __fastcall lua_combat_HasArtefact(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "HasArtefact");
	if (!hero) return 0;
	int artifact, equipped_only = 0;
	if (!lua_number_arg(state, 2, &artifact) || artifact < 0 || artifact >= ((CountFn)0x008478A0)()) {
		lua_error(state, "HasArtefact: invalid artefact id");
		return 0;
	}
	if (lua_has_arg(state, 3) && !lua_number_arg(state, 3, &equipped_only)) {
		lua_error(state, "HasArtefact: the third argument must be a number");
		return 0;
	}
	int* equipped = hero->inventory();
	bool found = ((HasItemFn)0x004BA880)(equipped, artifact) != 0;
	if (!found && !equipped_only) {
		int* backpack = hero->backpack();
		found = ((HasItemFn)0x005C06F0)(backpack, artifact) != 0;
	}
	lua_push_bool(state, found);
	return 1;
}

int __fastcall lua_combat_GetArtifactSetItemsCount(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "GetArtifactSetItemsCount");
	if (!hero) return 0;
	int set, equipped = 1;
	if (!lua_number_arg(state, 2, &set)) {
		lua_error(state, "GetArtifactSetItemsCount: invalid set id");
		return 0;
	}
	if (lua_has_arg(state, 3) && !lua_number_arg(state, 3, &equipped)) {
		lua_error(state, "GetArtifactSetItemsCount: the third argument must be a number");
		return 0;
	}
	int* sets = hero->artifact_sets();
	lua_push_number(state, ((SetCountFn)0x00B09FD0)(sets, set, equipped != 0));
	return 1;
}

int __fastcall lua_combat_KnowHeroSpell(int* state) {
	if (!state) return 0;
	IHero* hero = combat_hero_arg(state, "KnowHeroSpell");
	if (!hero) return 0;
	int spell;
	if (!lua_number_arg(state, 2, &spell)) {
		lua_error(state, "KnowHeroSpell: invalid spell id");
		return 0;
	}
	int checked = spell >= 0 && spell < ((CountFn)0x00847890)() ? spell : 0;
	if (!((IsSchoolSpellFn)0x00A349F0)(checked)) {
		lua_error(state, "KnowHeroSpell: invalid spell id #%i", spell);
		return 0;
	}
	lua_push_bool(state, hero->knows_spell(checked));
	return 1;
}
