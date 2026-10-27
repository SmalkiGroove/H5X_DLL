#include "pch.h"

// RMG GUARDS WITHOUT MONSTER FIX
// The random map generator guard pool only keeps creatures that have a map object (MonsterShared).
// func_MonsterSetter_PickCreature (0x00BA6910) keeps every creature 1..998 but 89 and 114 whose Power fits the guard
// power, then func_MonsterSetter_PlaceCreature (0x00BA6F00) places the picked one from its MonsterShared link: a
// creature without one gives an empty link, so no monster and a guardless treasure, without any log. Mod creatures with
// a Power and no MonsterShared (Druid of the Council, Avatar of Death, CREATURE_DARKSTORM_1..50) were picked that way.
// The candidate loop gets each creature's info at 0x00BA69EE (ecx = id, esi = id); the hook skips the creature when its
// MonsterShared (info +0xD4, resolved by 0x00A14540) is missing or its DB link (shared +0x0C: type string, byte, int,
// href string at +0x14 / +0x18) has an empty href, the link the CMonsterSetter constructor copies for PlaceCreature.
// See .cursor/rules/h5x-re-rmg.mdc.

void RmgGuardPoolFork();

int RmgGuardPool_fork = 0x00BA69EE;           // mov ecx,esi / call 0x00ADA2A0
int RmgGuardPool_return = 0x00BA69F5;         // town and power checks, eax = creature info
int RmgGuardPool_skip = 0x00BA6A60;           // next creature
int RmgGuardPool_creature_info = 0x00ADA2A0;

void RmgGuardPoolFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, RmgGuardPool_fork, 7, RmgGuardPoolFork, 0, 0, 0 });
}

typedef char*(__fastcall* ResolveSharedFn)(char*);
static ResolveSharedFn resolve_monster_shared = (ResolveSharedFn)0x00A14540;

static bool __fastcall has_monster_link(char* info) {
	char* shared = resolve_monster_shared(info + 0xD4);
	if (!shared) return false;
	char** link = (char**)(shared + 0x0C);
	return link[5] && link[6] > link[5];
}

__declspec(naked) void RmgGuardPoolFork() {
	__asm
	{
		mov ecx, esi
		call [RmgGuardPool_creature_info]
		push eax
		mov ecx, eax
		call has_monster_link
		test al, al
		pop eax
		je SKIP
		jmp [RmgGuardPool_return]
	SKIP:
		jmp [RmgGuardPool_skip]
	}
}
