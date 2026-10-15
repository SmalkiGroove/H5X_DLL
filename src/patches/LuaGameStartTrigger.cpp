#include "pch.h"

// LUA: GAME_START_TRIGGER (17), Trigger(GAME_START_TRIGGER, "func") runs func once for the whole game
// The world fires a trigger with CWorld vtable +0x21C (FireTrigger 0x009902B0, thiscall(name string, params vector),
// ret 8). Single player (world flags +0x88 multiplayer / +0x89 hotseat both 0): a CEventTrigger is queued and the
// display loop (0x005A5D90, event 0x24 -> 0x00607C20) runs "func(params)". Multiplayer / hotseat: CWorldScriptSystem
// (CWorld +0x1D0, 0x00990070) runs it at once. Either way it runs once per world, while the map script top-level code
// can run several times (twice in hotseat).
// Trigger type 17 doesn't register anything: lua_SetTrigger (0x005DD530) fires the trigger at once, through the same
// FireTrigger, with no parameter, once per world (the map script runs twice in hotseat). Calling it as the last line
// of the map scripts makes the callback run after all of them are loaded, on new and loaded games.

static const int game_start_trigger = 17;

void GameStartTriggerTypeFork();
void GameStartTriggerFireFork();

int GameStartTriggerRange_byte = 0x005DD75E;      // cmp ebp,11h: valid trigger types [0,16] -> [0,17]
int GameStartTriggerError_byte = 0x005DDF2D;      // push 10h: max type in the error message
int GameStartTriggerType_fork = 0x005DD76B;       // cmp ebp,10h / mov esi,eax / ja 0x005DDDFD (trigger switch)
int GameStartTriggerType_return = 0x005DD776;     // switch on the trigger type
int GameStartTriggerType_default = 0x005DDDFD;    // trigger object check (edi)
int GameStartTriggerType_found = 0x005DDE25;      // trigger object found: read the function name
int GameStartTriggerFire_fork = 0x005DDEC1;       // push 1Ch / call alloc (CSetTriggerActionCmd)
int GameStartTriggerFire_return = 0x005DDEC8;
int GameStartTriggerFire_end = 0x005DDF11;        // free the function name string, return

void LuaGameStartTrigger_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, GameStartTriggerRange_byte, 1, nullptr, game_start_trigger + 1, 0, 0 });
	assembly_patches.push_back({ PATCH_BYTE, GameStartTriggerError_byte, 1, nullptr, game_start_trigger, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, GameStartTriggerType_fork, 11, GameStartTriggerTypeFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, GameStartTriggerFire_fork, 7, GameStartTriggerFireFork, 0, 0, 0 });
}

static int* game_start_world = nullptr;   // world the trigger was fired for

// name = engine string {begin, end, capacity} of the function name (empty when nil)
// Fired once per world: the adventure map script can run more than once (twice in hotseat), and a new world (new or
// loaded game) fires it again.
static void __stdcall fire_game_start_trigger(int* world, char** name) {
	if (!world || !name[0] || name[1] == name[0]) return;
	if (world == game_start_world) return;
	game_start_world = world;
	int params[3] = { 0, 0, 0 };   // empty vector of trigger parameters
	typedef void(__thiscall* FireTriggerFn)(int*, char**, int*);
	((FireTriggerFn)(*(int***)world)[0x21C / 4])(world, name, params);
}

// eax = CWorld (IAdventureMap vtable +0x00), ebp = trigger type.
// Type 17 has no trigger object: the world goes in edi (checked non-null afterwards) and the object check is skipped.
__declspec(naked) void GameStartTriggerTypeFork() {
	__asm
	{
		mov esi, eax
		cmp ebp, game_start_trigger
		je GAME_START
		cmp ebp, 0x10
		ja DEFAULT
		jmp [GameStartTriggerType_return]
	DEFAULT:
		jmp [GameStartTriggerType_default]
	GAME_START:
		mov edi, esi
		jmp [GameStartTriggerType_found]
	}
}

// ebp = trigger type, edi = world for type 17, [esp+44h] = function name string.
// Type 17: fire the trigger instead of posting a CSetTriggerActionCmd.
__declspec(naked) void GameStartTriggerFireFork() {
	__asm
	{
		cmp ebp, game_start_trigger
		je GAME_START
		push 0x1C
		call [engine_alloc]
		jmp [GameStartTriggerFire_return]
	GAME_START:
		lea eax, [esp + 0x44]
		push eax
		push edi
		call fire_game_start_trigger
		jmp [GameStartTriggerFire_end]
	}
}
