#include "pch.h"
#include "types/Artifacts.h"
#include "types/HeroClasses.h"

// HAVEN ARTIFACT SET (11)
// 4: scripted
// 4 Haven: scripted
// 6: creatures always get good morale (good morale roll always succeeds, bad morale roll never does)
// 6 Haven: creatures get +2 attack and defense per point of positive morale
// Undead skip both morale rolls before the hooks (has_ability 10), so they are not affected.
// Set counts above 10 need ArtfsetExpand.

void ArtfsetHavenGoodMoraleFork();
void ArtfsetHavenBadMoraleFork();
void ArtfsetHavenAttackFork();
void ArtfsetHavenDefenseFork();

static const int haven6_pieces = 6;
static const int haven6_stat_per_morale = 2;

int ArtfsetHavenGoodMorale_fork = 0x00A6BA30;     // func_CombatCreature_CanCast (good morale roll), after the undead check
int ArtfsetHavenGoodMorale_return = 0x00A6BA36;
int ArtfsetHavenGoodMorale_success = 0x00A6BAF4;  // mov al,1; pop esi; add esp,8; ret
int ArtfsetHavenBadMorale_fork = 0x00A6B920;      // func_RollUnitMorale (bad morale roll), after the undead check
int ArtfsetHavenBadMorale_return = 0x00A6B926;
int ArtfsetHavenBadMorale_fail = 0x00A6B919;      // xor al,al; pop edi; add esp,8; ret
int ArtfsetHavenAttack_fork = 0x008A498C;         // func_GetUnitAttack, after the shooter bonuses
int ArtfsetHavenAttack_return = 0x008A4992;
int ArtfsetHavenDefense_fork = 0x008A23F8;        // func_GetUnitDefense, before the defend stance bonus
int ArtfsetHavenDefense_return = 0x008A23FF;

void ArtfsetHaven_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetHavenGoodMorale_fork, 6, ArtfsetHavenGoodMoraleFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetHavenBadMorale_fork, 6, ArtfsetHavenBadMoraleFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetHavenAttack_fork, 6, ArtfsetHavenAttackFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, ArtfsetHavenDefense_fork, 7, ArtfsetHavenDefenseFork, 0, 0, 0 });
}

static bool has_haven6(IHero* hero) {
	return hero && hero->artifact_set_count(ARTIFACT_SET_HAVEN) >= haven6_pieces;
}

static bool is_haven_class(IHero* hero) {
	int hero_class = hero->hero_class();
	return hero_class == HERO_CLASS_HAVEN_MIGHT || hero_class == HERO_CLASS_HAVEN_MAGIC;
}

static int __fastcall haven6_always_good_morale(ICombatUnit* unit) {
	return has_haven6(owner_hero(unit));
}

static int __fastcall haven6_stat_bonus(ICombatUnit* unit, int stat) {
	IHero* hero = owner_hero(unit);
	if (!has_haven6(hero) || !is_haven_class(hero)) return stat;
	int morale = unit->morale();
	if (morale > 0) stat += morale * haven6_stat_per_morale;
	return stat;
}

// esi = this (CombatUnit vbptr at this-0xB0)
__declspec(naked) void ArtfsetHavenGoodMoraleFork() {
	__asm
	{
		mov eax, dword ptr [esi - 0xB0]
		mov ecx, dword ptr [eax + 0x8]
		lea ecx, dword ptr [ecx + esi - 0xB0]
		call haven6_always_good_morale
		test eax, eax
		je HAVEN_GOOD_MORALE_ROLL
		jmp [ArtfsetHavenGoodMorale_success]

	HAVEN_GOOD_MORALE_ROLL:
		mov eax, dword ptr [esi - 0xB0]
		jmp [ArtfsetHavenGoodMorale_return]
	}
}

// edi = this (CombatUnit vbptr at this-0xB0)
__declspec(naked) void ArtfsetHavenBadMoraleFork() {
	__asm
	{
		mov eax, dword ptr [edi - 0xB0]
		mov ecx, dword ptr [eax + 0x8]
		lea ecx, dword ptr [ecx + edi - 0xB0]
		call haven6_always_good_morale
		test eax, eax
		je HAVEN_BAD_MORALE_ROLL
		jmp [ArtfsetHavenBadMorale_fail]

	HAVEN_BAD_MORALE_ROLL:
		mov eax, dword ptr [edi - 0xB0]
		jmp [ArtfsetHavenBadMorale_return]
	}
}

// esi = CombatUnit subobject (creature), ebx = attack
__declspec(naked) void ArtfsetHavenAttackFork() {
	__asm
	{
		mov ecx, esi
		mov edx, ebx
		call haven6_stat_bonus
		mov ebx, eax
		mov eax, dword ptr [esi - 0x144]
		jmp [ArtfsetHavenAttack_return]
	}
}

// esi = complete object (vbptr at esi+4), ebx = defense, [esp+0x14] = copy of ebx read by the defend stance bonus
__declspec(naked) void ArtfsetHavenDefenseFork() {
	__asm
	{
		mov eax, dword ptr [esi + 0x4]
		mov ecx, dword ptr [eax + 0x8]
		lea ecx, dword ptr [ecx + esi + 0x4]
		mov edx, ebx
		call haven6_stat_bonus
		mov ebx, eax
		mov dword ptr [esp + 0x14], ebx
		cmp byte ptr [esi + 0xCD], 0
		jmp [ArtfsetHavenDefense_return]
	}
}
