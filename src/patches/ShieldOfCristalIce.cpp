#include "pch.h"

// Shield of crystal ice immunes to ignite

void ShieldOfCrystalIceFork();

int ShieldOfCrystalIce_fork = 0x004E68B1;
int ShieldOfCrystalIce_return = 0x004E68BD;
int ShieldOfCrystalIce_end = 0x004E69CC;

void ShieldOfCrystalIce_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShieldOfCrystalIce_fork, 10, ShieldOfCrystalIceFork, 0, 0, 0 });
}

__declspec(naked) void ShieldOfCrystalIceFork() {
	__asm
	{
		test al, al
		je IGNITE_IMMUNITY_END

		mov ecx, dword ptr [ebx - 0x6C]
		mov eax, dword ptr [ecx]
		push 0x9
		call dword ptr [eax + 0x74]
		mov ecx, eax
		call[count_equipped_artifact]
		test eax, eax
		jne IGNITE_IMMUNITY_END
		
		mov edx, esi
		mov ecx, ebx
		jmp[ShieldOfCrystalIce_return]

		IGNITE_IMMUNITY_END:
		jmp[ShieldOfCrystalIce_end]
	}
}
