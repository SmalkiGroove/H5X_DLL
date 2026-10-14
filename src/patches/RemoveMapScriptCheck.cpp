#include "pch.h"

// Adventure map Lua start-up also runs on maps without a map script
// The map script loader 0x00992D00 (thiscall(map script object or 0), ret 4) builds the "advmap" script context, runs
// "scripts/advmap-startup.lua" and "createAdvmapAliases();", then the map's own script (object +0x44 / +0x48).
// Vanilla skips all of it when the map has no script (je at 0x00992D2A), so the mod's adventure map scripts never
// start there. The early exit is removed and the three uses of the map script object are guarded instead:
// - building the map script string (0x00992DB1),
// - running it (0x00992E43),
// - freeing it (0x00992E6F).

void MapScriptTextGuard();
void MapScriptRunGuard();
void MapScriptFreeGuard();

int MapScriptExit_site = 0x00992D2A;         // cmp [esp+34h],ebx (script object) / je 0x00992E7E
int MapScriptText_fork = 0x00992DB1;         // mov eax,[esp+3Ch] / mov ecx,[eax+48h]
int MapScriptText_return = 0x00992DB8;       // mov edx,[eax+44h]: map script string construction
int MapScriptText_skip = 0x00992DD2;         // push 8 / call alloc: after the string construction
int MapScriptRun_fork = 0x00992E43;          // mov eax,[esp+20h] / cmp eax,[esp+24h]
int MapScriptRun_return = 0x00992E4D;        // run the map script
int MapScriptRun_skip = 0x00992E65;          // free the temporary buffer
int MapScriptFree_fork = 0x00992E6F;         // mov edx,[esp+24h] / push edx / call free
int MapScriptFree_return = 0x00992E79;       // add esp,8 (both free arguments)
int MapScriptFree_skip = 0x00992E7C;         // pop edi / pop ebp / ... / ret 4

void RemoveMapScriptCheck_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, MapScriptExit_site, 6, nullptr, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, MapScriptText_fork, 7, MapScriptTextGuard, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, MapScriptRun_fork, 8, MapScriptRunGuard, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, MapScriptFree_fork, 10, MapScriptFreeGuard, 0, 0, 0 });
}

// [esp+3Ch] = map script object (the argument, after push ebp / push edi).
// No script: skip the map script string, its buffer [esp+20h..28h] stays unbuilt.
__declspec(naked) void MapScriptTextGuard() {
	__asm
	{
		cmp dword ptr [esp + 0x3C], 0
		je NO_SCRIPT
		mov eax, dword ptr [esp + 0x3C]
		mov ecx, dword ptr [eax + 0x48]
		jmp [MapScriptText_return]
	NO_SCRIPT:
		jmp [MapScriptText_skip]
	}
}

// No script, or an empty one ([esp+20h] begin == [esp+24h] end): don't run it.
__declspec(naked) void MapScriptRunGuard() {
	__asm
	{
		cmp dword ptr [esp + 0x3C], 0
		je SKIP
		mov eax, dword ptr [esp + 0x20]
		cmp eax, dword ptr [esp + 0x24]
		je SKIP
		jmp [MapScriptRun_return]
	SKIP:
		jmp [MapScriptRun_skip]
	}
}

// The argument of the previous free is still on the stack: script object at [esp+40h], string buffer at [esp+24h].
// No script: don't free the unbuilt string, and pop the pending argument here (the add esp,8 is skipped).
__declspec(naked) void MapScriptFreeGuard() {
	__asm
	{
		cmp dword ptr [esp + 0x40], 0
		je NO_SCRIPT
		mov edx, dword ptr [esp + 0x24]
		push edx
		call [engine_free]
		jmp [MapScriptFree_return]
	NO_SCRIPT:
		add esp, 4
		jmp [MapScriptFree_skip]
	}
}
