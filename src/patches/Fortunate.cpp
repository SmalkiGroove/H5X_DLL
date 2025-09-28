#include "pch.h"

// ABILITY BONUS LUCK 

void FortunateFork();

int Fortunate_fork = 0x00A6AC86;
int Fortunate_return = 0x00A6AC8B;

void Fortunate_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Fortunate_fork, 5, FortunateFork, 0, 0, 0 });
}

__declspec(naked) void FortunateFork() {
	__asm
	{
		mov eax, dword ptr [esi - 0xB0]
		mov ecx, dword ptr [eax + 0x8]
		mov edx, dword ptr [ecx + esi - 0xB0]
		lea ecx, dword ptr [ecx + esi - 0xB0]
		push 0x93
		call dword ptr [edx + 0x28C]
		test al, al
		je FORTUNATE_END
		add edi, 2

		FORTUNATE_END:
		mov ecx, ebx
		call[call_unknown_1]
		jmp[Fortunate_return]
	}
}
