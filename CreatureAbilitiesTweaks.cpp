#include "pch.h"

// EDIT CREATURE ABILITIES HARDCODED VALUES


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
		mov dword ptr ds : [edx + esi*8], edi
		mov dword ptr ds : [edx + esi*8 + 0x4], eax
		ret
	}
}

// Energy Channel reduce hero mana cost from 25% to 10%

void EnergyChannelTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT, 0x00400F40, 4, nullptr, 0, 0.90f, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x0097828C, 4, nullptr, 0, 0, 0, "40034000" });
}

// Paw Strike has 5x less probability to trigger

void PawStrikeFork();

int PawStrike_fork = 0x00A5D277;
int PawStrike_return = 0x00A5D27F;

void PawStrikeTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PawStrike_fork, 8, PawStrikeFork, 0, 0, 0 });
}

__declspec(naked) void PawStrikeFork() {
	__asm
	{
		mov edx, eax
		push ecx
		mov ecx, 0x5
		cdq
		idiv ecx
		pop ecx
		mov dword ptr ss : [esp + 0x10], eax
		fild dword ptr ss : [esp + 0x10]
		jmp[PawStrike_return]
	}
}
