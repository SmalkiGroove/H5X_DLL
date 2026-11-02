#include "pch.h"
#include "types/HeroSpecs.h"

// Overheater has the effect of the Exploding Corpses skill and ice-proof per level

void IceProofAbilityFork();

int IceProofAbility_fork = 0x0097B1DB;
int IceProofAbility_return = 0x0097B1E2;

void OverheaterTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x0097B8D1, 10, nullptr, 0, 0, 0, "6A5389F1FF9294020000" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0097BB2C, 10, nullptr, 0, 0, 0, "6A5389F1FF9094020000" });
	assembly_patches.push_back({ PATCH_WRTE, 0x0097BBE2, 1, nullptr, 0, 0, 0, "00" });
	assembly_patches.push_back({ PATCH_HOOK, IceProofAbility_fork, 7, IceProofAbilityFork, 0, 0, 0 });
}


// 1 - level%, with the x87 instructions of the original code (same rounding at the game's FPU precision)
static float one_minus_percent(int level) {
	float result;
	__asm {
		fild dword ptr [level]
		fmul dword ptr [constf_percent]
		fsubr dword ptr [constf_1]
		fstp dword ptr [result]
	}
	return result;
}

// Ice damage taken by an Overheater hero's units: x (1 - level%)
static void __fastcall overheater_ice_proof(IHero* hero, float* damage_factor) {
	if (!hero->has_spec(HERO_SPEC_INFERNAL_MACHINE)) return;
	*damage_factor *= one_minus_percent(hero->level());
}

// ebx = defending IHero, [esp+0x4C] = damage factor
__declspec(naked) void IceProofAbilityFork() {
    __asm
    {
		lea edx, [esp + 0x4C]
		mov ecx, ebx
		call overheater_ice_proof

		mov edx, [ebx]
		mov ecx, ebx
		call dword ptr [edx]Hero_vtable.get_inventory
		jmp [IceProofAbility_return]
    }
}
