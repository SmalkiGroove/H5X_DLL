#include "pch.h"
#include "types/HeroSpecs.h"

// Enlightened gain bonus experience

void EnlightenedFork();

int Enlightened_fork = 0x00B4BBE7;
int Enlightened_return = 0x00B4BBFE;
int Enlightened_end = 0x00B4BC0B;

void Enlightened_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Enlightened_fork, 23, EnlightenedFork, 0, 0, 0 });
}

static bool __fastcall enlightened_bonus_experience(IHero* hero) {
	return hero->has_spec(HERO_SPEC_DARK_ACOLYTE);
}

// esi = IHero
__declspec(naked) void EnlightenedFork() {
	__asm
	{
		lea eax, dword ptr [eax + eax*4]
		lea edi, dword ptr [edi + eax*4]

		mov ecx, esi
		call enlightened_bonus_experience
		test al, al
		jz ENLIGHTENED_END
		jmp [Enlightened_return]

	ENLIGHTENED_END:
		jmp [Enlightened_end]
	}
}
