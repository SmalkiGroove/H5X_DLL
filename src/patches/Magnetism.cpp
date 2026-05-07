#include "pch.h"

// ABILITY REDIRECT PROJECTILES

void MagnetismFork();

int Magnetism_fork = 0x00A65EAA;
int Magnetism_return = 0x00A65EB1;
int Magnetism_end = 0x00A65FA5;
int Magnetism_trigger = 0x00A65ED8;

void Magnetism_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Magnetism_fork, 7, MagnetismFork, 0, 0, 0 });
}

__declspec(naked) void MagnetismFork() {
	__asm
	{
		mov ebx, dword ptr ss : [esp + 0x9C]
		cmp ebx, edi
		je MAGNETISM_END

		mov eax, dword ptr [esi]
		mov ecx, esi
		call dword ptr [eax + 0x6C]
		mov ebp, eax

		mov eax, dword ptr [ebx]
		mov ecx, ebx
		push 0x94
		call dword ptr [eax + 0x28C]
		test al, al
		je MAGNETISM_END
		jmp[Magnetism_trigger]


		MAGNETISM_RETURN:
		jmp[Magnetism_return]

		MAGNETISM_END:
		jmp[Magnetism_end]
	}
}
