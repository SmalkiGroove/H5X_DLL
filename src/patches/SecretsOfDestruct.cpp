#include "pch.h"


// Secrets of destruction negates magic disruption entirely

void SecretsOfDestructFork();

int SecretsOfDestruct_fork = 0x0097CF8D;
int SecretsOfDestruct_return = 0x0097CF93;
int SecretsOfDestruct_end = 0x0097D0B0;

void SecretsOfDestruct_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, SecretsOfDestruct_fork, 6, SecretsOfDestructFork, 0, 0, 0 });
}

// hero = casting hero (ICombatHeroInterface, vbptr at +4)
static bool __fastcall secrets_of_destruction(int* hero) {
	return combat_creature_detail::combat_hero_from_outer(hero)->has_perk(PERK_SECRETS_OF_DESTRUCT);
}

// ebx = casting hero (ICombatHeroInterface)
__declspec(naked) void SecretsOfDestructFork() {
	__asm
	{
		mov ecx, ebx
		call secrets_of_destruction
		test al, al
		je SECRETS_OF_DESTRUCT_RETURN
		jmp [SecretsOfDestruct_end]

	SECRETS_OF_DESTRUCT_RETURN:
		mov edx, dword ptr [ebx]
		mov ecx, ebx
		call dword ptr [edx]CombatHeroInterface_vtable.get_hero
		jmp [SecretsOfDestruct_return]
	}
}
