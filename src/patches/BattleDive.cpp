#include "pch.h"

// Battle Dive damages reduced from 200% to 150%

void BattleDiveFork();

void BattleDiveTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_CALL, 0x00A5FE81, 7, BattleDiveFork, 0, 0, 0 });
}

__declspec(naked) void BattleDiveFork() {
	__asm
	{
		cmp ebx, 0x9B
		jne BATTLE_DIVE_DAMAGE
		mov eax, 0x3F400000
		BATTLE_DIVE_DAMAGE:
		nop
			mov dword ptr[edx + esi * 8], edi
			mov dword ptr[edx + esi * 8 + 0x4], eax
			ret
	}
}
