#include "pch.h"


// Secrets of destruction negates magic disruption entirely

void SecretsOfDestructFork();

int SecretsOfDestruct_fork = 0x0097CF8D;
int SecretsOfDestruct_return = 0x0097CF93;
int SecretsOfDestruct_end = 0x0097D0B0;

void SecretsOfDestruct_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, SecretsOfDestruct_fork, 6, SecretsOfDestructFork, 0, 0, 0 });
}

__declspec(naked) void SecretsOfDestructFork() {
	__asm
	{
		mov eax, [ebx + 0x4]
        mov ecx, [eax + 0x8]
        mov edx, [ecx + ebx + 0x4]
        lea ecx, [ecx + ebx + 0x4]
        push PERK_SECRETS_OF_DESTRUCT
        call dword ptr [edx + 0x290]
        test al, al
        je SECRETS_OF_DESTRUCT_RETURN
		jmp[SecretsOfDestruct_end]
		
		SECRETS_OF_DESTRUCT_RETURN:
		mov edx, dword ptr [ebx]
		mov ecx, ebx
        call dword ptr [edx]
		jmp[SecretsOfDestruct_return]
	}
}
