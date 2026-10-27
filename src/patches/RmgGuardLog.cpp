#include "pch.h"
#include <intrin.h>

// RMG GUARD LOG (diagnostic)
// Logs in H5.log every guard the random map generator makes, and whether a monster object was created, to find the
// treasures left without guard. See .cursor/rules/h5x-re-rmg.mdc.
// func_MonsterSetter_MakeGuard (0x00BA7690, __thiscall CMonsterSetter, args: out DB link, power, float* tile, rotation;
// ret 0x10) makes every RMG guard (connections, mines, dwellings, treasure blocks). Power < 100 makes nothing; else the
// power is scaled by the monster level, then either a custom army template (0x00BA71B0 / 0x00BA7040) or one creature
// picked by power (0x00BA6910: any creature 1..998 but 89 and 114 whose Power fits, with its count) placed by 0x00BA6F00
// from the setter's creature -> MonsterShared link map. A creature without MonsterShared has an empty link there and
// gives no object, without any log.
// The out link is { type string (begin, end, cap), byte, int, href string (begin +0x14, end +0x18) }: no monster when
// the href is empty. The entry hook wraps the whole function; the two call hooks record the pick and the custom army.

int RmgGuard_fork = 0x00BA7690;            // mov eax,[esp+8] / sub esp,34h
int RmgGuard_resume = 0x00BA7697;
int RmgGuardPick_fork = 0x00BA798A;        // call 0x00BA6910 (creature pick)
int RmgGuardArmy_fork = 0x00BA7924;        // call 0x00BA7040 (custom army template)

typedef char(__fastcall* PickCreatureFn)(int*, int, int, int, int*, int*, int*);
typedef int*(__fastcall* MakeArmyGuardFn)(int*, int, int*, int*, float*, int);
typedef int*(__fastcall* MakeGuardFn)(int*, int, char**, int, float*, int);

static PickCreatureFn pick_creature = (PickCreatureFn)0x00BA6910;
static MakeArmyGuardFn make_army_guard = (MakeArmyGuardFn)0x00BA7040;

static int last_creature = -1;
static int last_count = 0;
static int last_pick = -1;      // -1 not called, 0 failed, 1 picked
static bool last_army = false;

void MakeGuardOriginal();
int* __fastcall make_guard_logged(int* setter, int edx, char** out, int power, float* tile, int rotation);
char __fastcall pick_creature_logged(int* setter, int edx, int town, int power, int* total, int* count, int* creature);
int* __fastcall make_army_guard_logged(int* setter, int edx, int* out, int* army, float* tile, int rotation);

void RmgGuardLog_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, RmgGuard_fork, 7, (void*)make_guard_logged, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, RmgGuardPick_fork, 5, (void*)pick_creature_logged, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, RmgGuardArmy_fork, 5, (void*)make_army_guard_logged, 0, 0, 0 });
}

// The 7 overwritten bytes, then the rest of func_MonsterSetter_MakeGuard (same stack as a __fastcall with 4 stack args).
__declspec(naked) void MakeGuardOriginal() {
	__asm
	{
		mov eax, dword ptr [esp + 8]
		sub esp, 0x34
		jmp [RmgGuard_resume]
	}
}

char __fastcall pick_creature_logged(int* setter, int edx, int town, int power, int* total, int* count, int* creature) {
	char picked = pick_creature(setter, edx, town, power, total, count, creature);
	last_pick = picked ? 1 : 0;
	last_creature = picked ? *creature : -1;
	last_count = picked ? *count : 0;
	return picked;
}

int* __fastcall make_army_guard_logged(int* setter, int edx, int* out, int* army, float* tile, int rotation) {
	last_army = true;
	return make_army_guard(setter, edx, out, army, tile, rotation);
}

int* __fastcall make_guard_logged(int* setter, int edx, char** out, int power, float* tile, int rotation) {
	void* caller = _ReturnAddress();
	last_creature = -1;
	last_count = 0;
	last_pick = -1;
	last_army = false;
	int* result = ((MakeGuardFn)(void*)MakeGuardOriginal)(setter, edx, out, power, tile, rotation);

	bool created = out[5] && out[6] > out[5];
	std::string what;
	if (power < 100) what = "power below 100";
	else if (last_army) what = "custom army";
	else if (last_pick == 0) what = "no creature fits the power";
	else what = "creature " + std::to_string(last_creature) + " x" + std::to_string(last_count);
	char ret[16];
	sprintf_s(ret, "%08X", (unsigned int)(uintptr_t)caller);
	writeLog(created ? INFO : WARNING, std::string("RmgGuard: ") + (created ? "placed" : "NO MONSTER")
		+ " at " + std::to_string((int)tile[0]) + ":" + std::to_string((int)tile[1])
		+ " floor " + std::to_string(*(int*)((char*)setter + 0x2C))
		+ ", power " + std::to_string(power) + ", " + what
		+ (created ? ", shared \"" + std::string(out[5], out[6]) + "\"" : std::string())
		+ ", caller " + ret);
	return result;
}
