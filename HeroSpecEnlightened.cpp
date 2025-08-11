#include "pch.h"

// Enlightened gain bonus experience

void EnlightenedFork();

int Enlightened_fork = 0x00B4BBE7;
int Enlightened_return = 0x00B4BBFE;
int Enlightened_end = 0x00B4BC0B;

void Enlightened_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Enlightened_fork, 23, EnlightenedFork, 0, 0, 0 });
}

__declspec(naked) void EnlightenedFork() {
	__asm
	{
	    lea eax, dword ptr [eax + eax*4]
		lea edi, dword ptr [edi + eax*4]

	    mov eax, dword ptr [esi]
	    mov ecx, esi
		push 0x33
		call dword ptr [eax + 0x190]
		test al, al
		jz ENLIGHTENED_END
		jmp[Enlightened_return]

	ENLIGHTENED_END:
		jmp[Enlightened_end]
	}
}
