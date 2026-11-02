#include "pch.h"
#include "types/Abilities.h"

// Fix: creatures with Unlimited Retaliations (ability 6) and buff 286 (0x11E)
// should retaliate before the attack only, not again after the hit.

void PreparationUnlimRetalFixFork();

int PreparationUnlimRetalFix_fork = 0x00A6506C;
int PreparationUnlimRetalFix_continue = 0x00A65073;
int PreparationUnlimRetalFix_skip = 0x00A650E9;

void PreparationUnlimRetalFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PreparationUnlimRetalFix_fork, 7, PreparationUnlimRetalFixFork, 0, 0, 0 });
}

// before_attack = the retaliation already happened before the attack (effect 286 / 0x11E path)
static bool __fastcall skip_post_hit_retaliation(ICombatUnit* retaliator, bool before_attack) {
	return before_attack && retaliator->has_ability(ABILITY_UNLIMITED_RETALIATION);
}

// edi = retaliator; [esp+0x58] = retaliated before the attack, [esp+0x40] = vanilla retaliation flag
__declspec(naked) void PreparationUnlimRetalFixFork() {
	__asm {
		cmp byte ptr [esp + 0x58], 0x0
		setne dl
		mov ecx, edi
		call skip_post_hit_retaliation
		test al, al
		jne UNLIMITED_RETAL_286_SKIP

		cmp byte ptr [esp + 0x40], 0x0
		jz UNLIMITED_RETAL_286_SKIP
		jmp [PreparationUnlimRetalFix_continue]

	UNLIMITED_RETAL_286_SKIP:
		jmp [PreparationUnlimRetalFix_skip]
	}
}
