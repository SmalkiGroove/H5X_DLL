#include "pch.h"

// EARTH SPELLS IGNORE HALF OF ENEMY'S SPELL PROOF

void EarthPierceFork();

int EarthPierce_fork = 0x0097D42C;
int EarthPierce_return = 0x0097D432;
int EarthPierce_end = 0x0097D47F;

void EarthPierce_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, EarthPierce_fork, 6, EarthPierceFork, 0, 0, 0 });
}

__declspec(naked) void EarthPierceFork() {
	__asm
	{
        mov ecx, ebp
        call[get_spell_element]
		cmp eax, 0x4
		jne EARTH_PIERCE_RETURN
        fld dword ptr ss: [esp + 0x10]
        fmul dword ptr [constf_0_5]
        fstp dword ptr ss: [esp + 0x10]

	EARTH_PIERCE_RETURN:
		test edi, edi
		je EARTH_PIERCE_END
		mov edx, dword ptr [edi]
		jmp[EarthPierce_return]

	EARTH_PIERCE_END:
		jmp[EarthPierce_end]
	}
}
