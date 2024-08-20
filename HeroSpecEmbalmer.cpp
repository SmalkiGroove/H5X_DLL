#include "pch.h"

// Embalmer formula reworked
// heal + ((level+4)/2)²

void EmbalmerFork();

int Embalmer_fork = 0x0097721E;
int Embalmer_return = 0x00977223;

void EmbalmerTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Embalmer_fork, 5, EmbalmerFork, 0, 0, 0 });
}

__declspec(naked) void EmbalmerFork() {
	__asm
	{
		add eax, 0x4
		sar eax, 0x1
		mul eax
		add[edi], eax
		jmp[Embalmer_return]
	}
}
