#include "pch.h"

// ABILITY REDIRECT PROJECTILES

void MagnetismFork();

int Magnetism_fork = 0x00A65EAA;
int Magnetism_return = 0x00A65EB1;
int Magnetism_end = 0x00A65FA5;

void Magnetism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Magnetism_fork, 7, MagnetismFork, 0, 0, 0 });
}

__declspec(naked) void MagnetismFork() {
	__asm
	{
		cmp ebx, edi
		je MAGNETISM_END


		MAGNETISM_RETURN:
		mov ebx, dword ptr ss : [esp + 0x98]
		jmp[Magnetism_return]

		MAGNETISM_END:
		jmp[Magnetism_end]
	}
}
