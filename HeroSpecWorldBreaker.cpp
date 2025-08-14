#include "pch.h"

// Troops get immunity to earth

void WorldBreakerFork();

int WorldBreaker_fork = 0x008A37ED;
int WorldBreaker_return = 0x008A37F2;
int WorldBreaker_next = 0x008A3838;

void WorldBreaker_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, WorldBreaker_fork, 5, WorldBreakerFork, 0, 0, 0 });
}

__declspec(naked) void WorldBreakerFork() {
	__asm
	{
		cmp esi, 0x17
		jne WORLDBREAKER_SKIP
		mov edx, dword ptr ss : [ebp - 0x144]
		mov eax, dword ptr ds : [edx + 0x8]
		mov edx, dword ptr ds : [eax + ebp - 0x144]
		lea ecx, dword ptr ds : [eax + ebp - 0x144]
		call dword ptr ds : [edx + 0xC]
		mov edx, dword ptr ds : [eax]
		mov ecx, eax
		call dword ptr ds : [edx + 0xC]
		cmp eax, edi
		je WORLDBREAKER_NEXT
		mov ecx, dword ptr ds : [eax + 0x4]
		mov edx, dword ptr ds : [ecx + 0x8]
		lea ecx, dword ptr ds : [edx + eax + 0x4]
		mov eax, dword ptr ds : [ecx]
		push 0x2A
		call dword ptr ds : [eax + 0x294]
		test al, al
		je WORLDBREAKER_NEXT
		pop edi
		pop esi
		pop ebp
		pop ebx
		add esp, 0x1C
		ret 4

	WORLDBREAKER_SKIP:
		cmp esi, 0x28
		jne WORLDBREAKER_NEXT
		jmp[WorldBreaker_return]

	WORLDBREAKER_NEXT:
		jmp[WorldBreaker_next]
	}
}
