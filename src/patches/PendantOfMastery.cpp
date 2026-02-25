#include "pch.h"

// 

void PendantOfMastery_wrapper();

int PendantOfMastery_fork = 0x00B4D181;
int PendantOfMastery_return = 0x00B4D19C;

void PendantOfMastery_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, PendantOfMastery_fork, 27, PendantOfMastery_wrapper, 0, 0, 0 });
}

int faction_skills[] = { 4, 14, 15, 16, 17, 18, 151, 172 };
bool is_faction_skill(int skill_id) {
	for (int i = 0; i < sizeof(faction_skills) / sizeof(faction_skills[0]); i++) {
		if (faction_skills[i] == skill_id) return true;
	}
	return false;
}
int __fastcall pendantofmastery_check(IHero* hero, int skill_id) {
	if (is_faction_skill(skill_id)) {
		int result = 0;
		int* inventory = hero->instance->get_inventory();
		result += count_equipped_artifact(inventory, ARTIFACT_PENDANT_OF_MASTERY);
		int* skilltree = offsetPointer((int*)hero, -7);
		result += skill_mastery(skilltree, PERK_ABSOLUTE_MASTERY);
		return result;
	}
	return 0;
}
__declspec(naked) void PendantOfMastery_wrapper() {
	__asm
	{
		test eax, eax
		je ABSOLUTE_MASTERY_RETURN
		mov edx, ebx
		mov ecx, esi
		call[pendantofmastery_check]
		ABSOLUTE_MASTERY_RETURN:
		jmp[PendantOfMastery_return]
	}
}
