#include "pch.h"
#include "types/Abilities.h"

// ABILITY BONUS LUCK
// Natural Luck (ability 147): +2 luck

void FortunateFork();

int Fortunate_fork = 0x00A6AC86;
int Fortunate_return = 0x00A6AC8B;

void Fortunate_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Fortunate_fork, 5, FortunateFork, 0, 0, 0 });
}

// object = the luck function's this (esi): its CombatUnit is reached through the vbptr at -0xB0
static int __fastcall fortunate_luck_bonus(char* object) {
	char* base = object - 0xB0;
	ICombatUnit* unit = (ICombatUnit*)(base + (*(int**)base)[2]);
	return unit->has_ability(ABILITY_NATURAL_LUCK) ? 2 : 0;
}

// edi = luck
__declspec(naked) void FortunateFork() {
	__asm
	{
		mov ecx, esi
		call fortunate_luck_bonus
		add edi, eax

		mov ecx, ebx
		call [check_week_of_balance_luck]
		jmp [Fortunate_return]
	}
}
