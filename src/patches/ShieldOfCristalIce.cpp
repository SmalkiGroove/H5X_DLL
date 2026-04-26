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

		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax + 0xC]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0xC]
		test eax, eax
		je IGNITE_IMMUNITY_RETURN
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx]
		mov edx, dword ptr [eax]
		mov ecx, eax
		call dword ptr [edx + 0x74]
		mov ecx, eax
		push 0x9
		call[count_equipped_artifact]
		test eax, eax
		jne IGNITE_IMMUNITY_END
		
		IGNITE_IMMUNITY_RETURN:
		mov edx, esi
		mov ecx, ebx
		jmp[ShieldOfCrystalIce_return]

		IGNITE_IMMUNITY_END:
		jmp[ShieldOfCrystalIce_end]
	}
}
