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

__declspec(naked) void PreparationUnlimRetalFixFork() {
	__asm {
		// In this frame, [esp+0x58] is the local flag set by the
		// "retaliate-before-attack" (effect 286 / 0x11E) path.
		cmp byte ptr [esp + 0x58], 0x0
		je UNLIMITED_RETAL_286_CONTINUE

		// If the retaliator has Unlimited Retaliations (ability 6),
		// skip the post-hit retaliation branch.
		mov edx, dword ptr [edi]
		mov ecx, edi
		push ABILITY_UNLIMITED_RETALIATION
		call dword ptr [edx + 0x28C]
		test al, al
		jne UNLIMITED_RETAL_286_SKIP

		UNLIMITED_RETAL_286_CONTINUE:

		cmp byte ptr [esp + 0x40], 0x0
		jz UNLIMITED_RETAL_286_SKIP
		jmp[PreparationUnlimRetalFix_continue]

		UNLIMITED_RETAL_286_SKIP:
		jmp[PreparationUnlimRetalFix_skip]
	}
}
