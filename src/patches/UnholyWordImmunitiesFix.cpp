#include "pch.h"
#include "types/Abilities.h"
#include "Artfset.h"

// CALL OF THE NETHERWORLD SHOULD NOT HIT ORCS AND WAR MACHINES
// Also: creatures of a hero with 6 pieces of the Dungeon set (ArtfsetDungeon.cpp).

// 0x00977600, __fastcall(ecx = unit, edx = spell ID) -> al = 1 when the unit is immune to the spell:
// SPELL_UNHOLY_WORD (Curse of the Netherworld) skips undead (10) and demonic (11) units,
// SPELL_HOLY_WORD only hits units with abilities 105 / 10 / 11.

void UnholyWordImmunitiesFork();

int UnholyWordImmunities_fork = 0x00977643;    // SPELL_UNHOLY_WORD case: mov edx,[esi] / push 0Ah / mov ecx,esi
int UnholyWordImmunities_return = 0x00977649;  // call [edx+28Ch] (undead)
int UnholyWordImmunities_end = 0x0097763F;     // mov al,1 (immune)

void UnholyWordImmunitiesFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, UnholyWordImmunities_fork, 6, UnholyWordImmunitiesFork, 0, 0, 0 });
}

static bool unit_has_ability(ICombatUnit* unit, int ability) {
	return (char)unit->instance->has_ability((int*)unit, ability) != 0;  // only al is set
}

static int __fastcall unholy_word_immune(ICombatUnit* unit) {
	return unit_has_ability(unit, ABILITY_DEMON_RAGED)   // 0x69, orcs (demon blood)
		|| unit_has_ability(unit, ABILITY_MECHANICAL)     // war machines
		|| artfset_dungeon_unholy_word_immune(unit);
}

// esi = unit
__declspec(naked) void UnholyWordImmunitiesFork() {
	__asm
	{
		mov ecx, esi
		call unholy_word_immune
		test eax, eax
		jnz UNHOLY_WORD_IMMUNE

		mov edx, dword ptr [esi]
		push 0xA
		mov ecx, esi
		jmp [UnholyWordImmunities_return]

	UNHOLY_WORD_IMMUNE:
		jmp [UnholyWordImmunities_end]
	}
}
