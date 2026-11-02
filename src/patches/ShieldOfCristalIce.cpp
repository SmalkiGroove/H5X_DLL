#include "pch.h"

// Shield of crystal ice immunes to ignite

void ShieldOfCrystalIceFork();

int ShieldOfCrystalIce_fork = 0x004E68B1;
int ShieldOfCrystalIce_return = 0x004E68BD;
int ShieldOfCrystalIce_end = 0x004E69CC;

void ShieldOfCrystalIce_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, ShieldOfCrystalIce_fork, 10, ShieldOfCrystalIceFork, 0, 0, 0 });
}

// target = unit hit by Ignite: immune when its own hero wears the shield
static bool __fastcall shield_of_crystal_ice_protects(ICombatUnit* target) {
	IHero* hero = owner_hero(target);
	return hero && hero->has_artifact(ARTIFACT_SHIELD_OF_CRYSTAL_ICE);
}

// al = vanilla check result, esi = target unit
__declspec(naked) void ShieldOfCrystalIceFork() {
	__asm
	{
		test al, al
		je IGNITE_IMMUNITY_END

		mov ecx, esi
		call shield_of_crystal_ice_protects
		test al, al
		jne IGNITE_IMMUNITY_END

		mov edx, esi
		mov ecx, ebx
		jmp [ShieldOfCrystalIce_return]

	IGNITE_IMMUNITY_END:
		jmp [ShieldOfCrystalIce_end]
	}
}
