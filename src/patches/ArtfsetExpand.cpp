#include "pch.h"
#include "types/Artifacts.h"
#include "Artfset.h"

// Increase number of artifact sets to ARTIFACT_SET_MAX (40)
// The hero keeps one equipped-piece count per set in a vector (IHero [this+0x264], CHero +0x280), created with
// 10 entries by the two CHero constructors and rebuilt with 10 entries by the equip handler FUN_00B51590,
// which also skips set IDs > 10. The getter (IHero vtable +0x328) is replaced by a bounds-checked version so a
// vector still holding 10 entries (e.g. restored from an older save) reads 0 instead of past its end.

// Set effects applied when the hero's equipment changes (Artfset.h): func_CheckArtifactPassiveEffects (0x00B5A410)
// runs after every equip / unequip, once FUN_00B51590 has rebuilt the set counts, and ends with the spellbook
// refresh. The set effects run just before that refresh.

void ArtifactSetCountFork();
void ArtfsetEquipFork();

int ArtifactSetCount_fork = 0x00B5AD70;  // IHero vtable +0x328
int ArtfsetEquip_fork = 0x00B5A4AA;      // func_CheckArtifactPassiveEffects: call FUN_00b53b10 (spellbook refresh), ecx = CHero (IHero - 0x1C)
int ArtfsetEquip_refresh = 0x00B53B10;

void ArtfsetExpand_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x00B58C99, 1, nullptr, ARTIFACT_SET_MAX, 0, 0, 0 });  // CHero ctor: push 0Ah (vector size)
	assembly_patches.push_back({ PATCH_BYTE, 0x00B5910D, 1, nullptr, ARTIFACT_SET_MAX, 0, 0, 0 });  // CHero ctor: push 0Ah (vector size)
	assembly_patches.push_back({ PATCH_BYTE, 0x00B515ED, 1, nullptr, ARTIFACT_SET_MAX, 0, 0, 0 });  // equip handler: push 0Ah (vector size)
	assembly_patches.push_back({ PATCH_BYTE, 0x00B5164B, 1, nullptr, ARTIFACT_SET_MAX, 0, 0, 0 });  // equip handler: cmp eax, 0Ah (max set ID)
	assembly_patches.push_back({ PATCH_HOOK, ArtifactSetCount_fork, 6, ArtifactSetCountFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, ArtfsetEquip_fork, 5, ArtfsetEquipFork, 0, 0, 0 });
}

// Equipped pieces of a set, 0 for an ID outside the hero's count vector.
static int __fastcall artifact_set_count(int* hero, int set_id) {
	int* first = *(int**)(hero + 0x264 / 4);
	int* last = *(int**)(hero + 0x268 / 4);
	if (set_id < 1 || !first || set_id > last - first) return 0;
	return first[set_id - 1];
}

static void __fastcall artfset_equip_effects(char* chero) {
	IHero* hero = (IHero*)(chero + 0x1C);
	artfset_sylvan_teach_spells(hero);
	artfset_dungeon_teach_spells(hero);
}

// Replaces the call to the spellbook refresh (__fastcall, ecx = CHero): set effects, then the refresh itself
// (tail jump, it returns to func_CheckArtifactPassiveEffects).
__declspec(naked) void ArtfsetEquipFork() {
	__asm
	{
		push ecx
		call artfset_equip_effects
		pop ecx
		jmp [ArtfsetEquip_refresh]
	}
}

// __thiscall int (int set_id), replaces the whole game function
__declspec(naked) void ArtifactSetCountFork() {
	__asm
	{
		mov edx, dword ptr [esp + 0x4]
		call artifact_set_count
		ret 0x4
	}
}

// Change id of vanilla sets
/*
void ArtfsetRepoint_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB240, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB373, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x004BC55A, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 4
	assembly_patches.push_back({ PATCH_BYTE, 0x00B50538, 1, nullptr, 31, 0, 0, 0 }); // Dragonish 8
	assembly_patches.push_back({ PATCH_BYTE, 0x0097B393, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 2
	assembly_patches.push_back({ PATCH_BYTE, 0x00B4CD16, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 2*
	assembly_patches.push_back({ PATCH_BYTE, 0x00BC1381, 1, nullptr, 32, 0, 0, 0 }); // Dwarven 4
  //assembly_patches.push_back({ PATCH_BYTE, 0x008CF811, 1, nullptr, 33, 0, 0, 0 }); // Lions 2* -> spec Infantry Commander
	assembly_patches.push_back({ PATCH_BYTE, 0x00BBFC06, 1, nullptr, 33, 0, 0, 0 }); // Lions 3
  //assembly_patches.push_back({ PATCH_BYTE, 0x004BAB47, 1, nullptr, 34, 0, 0, 0 }); // Sar-issus 2 -> spec Shaman Matriarch
  //assembly_patches.push_back({ PATCH_INT,  0x008A3B02, 4, nullptr, 34, 0, 0, 0 }); // Sar-issus 2 -> spec Shaman Matriarch
	assembly_patches.push_back({ PATCH_BYTE, 0x00BBF8E8, 1, nullptr, 34, 0, 0, 0 }); // Sar-issus 4 & 2*
	assembly_patches.push_back({ PATCH_INT,  0x008A2064, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2
	assembly_patches.push_back({ PATCH_INT,  0x004E43C7, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x00989D0F, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x0098A1F3, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_INT,  0x009BC587, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 2*
	assembly_patches.push_back({ PATCH_BYTE, 0x00C25EB8, 1, nullptr, 35, 0, 0, 0 }); // Necromancers 4
	assembly_patches.push_back({ PATCH_BYTE, 0x00C262C5, 1, nullptr, 35, 0, 0, 0 }); // Necromancers 4
	assembly_patches.push_back({ PATCH_INT,  0x008A8161, 4, nullptr, 35, 0, 0, 0 }); // Necromancers 4
  //assembly_patches.push_back({ PATCH_BYTE, 0x00B4BBED, 1, nullptr, 36, 0, 0, 0 }); // Educational 2 -> spec Enlightened
  //assembly_patches.push_back({ PATCH_INT,  0x00A65C39, 4, nullptr, 37, 0, 0, 0 }); // Hunters 2 -> spec Tempress
  //assembly_patches.push_back({ PATCH_BYTE, 0x008CE90B, 1, nullptr, 37, 0, 0, 0 }); // Hunters 2* -> spec Lone Hunter
	assembly_patches.push_back({ PATCH_BYTE, 0x004BC576, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2
	assembly_patches.push_back({ PATCH_BYTE, 0x004BB25C, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2
	assembly_patches.push_back({ PATCH_BYTE, 0x008CE933, 1, nullptr, 38, 0, 0, 0 }); // Ogres 2*
  //assembly_patches.push_back({ PATCH_INT,  0x004B0E55, 4, nullptr, 39, 0, 0, 0 }); // Runic 2* -> spec Elementalist
	assembly_patches.push_back({ PATCH_BYTE, 0x008603AA, 1, nullptr, 40, 0, 0, 0 }); // Demonic 2*
}
*/