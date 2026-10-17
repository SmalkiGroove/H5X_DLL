#include "pch.h"

// Fix: loading a save that contains empty windows crashed at 0x00A4E002 (eax = 0)
// FUN_00A4DFF0 is the window rect getter shared by the window classes (reached through vtordisp thunks):
// rect = { x, y, x + width, y + height }, with the position at this-0xA8 / this-0xA4 and the size at +0xA4 / +0xA8 of
// the window's Shared object, returned by slot 2 of the vtable at this-0x10C (CWindowSimple: [window+0xF8]).
// The adventure screen (CWindowScreen) keeps a list of the button groups of its windows, as (ButtonGroupID, parent
// window) pairs. Buttons with a group inside an engine dialog (the H5X level-up box used to embed the encyclopedia)
// add their parents to that list, and the entries outlive the dialog. The save writes those dead windows as released
// objects (flag 0 in its object table), the load re-creates them as empty CWindowSimple (no parent, no DB instance,
// no Shared), and the layout pass of the screen's activation (FUN_00488220) reads their size.
// A window without a Shared now gets an empty rect at its position. Saves made before the H5X data fix contain such
// windows; new saves should not.

void EmptyWindowRectFork();

int EmptyWindowRect_fork = 0x00A4E002;      // fld dword ptr [eax+0A8h], eax = Shared returned by [vtable+8]
int EmptyWindowRect_return = 0x00A4E008;

void EmptyWindowRectFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, EmptyWindowRect_fork, 6, EmptyWindowRectFork, 0, 0, 0 });
}

// esi = this, [esp] = saved esi, [esp+4] = return address, [esp+8] = rect to fill
__declspec(naked) void EmptyWindowRectFork() {
	__asm
	{
		test eax, eax
		je NO_SHARED
		fld dword ptr [eax + 0xA8]
		jmp [EmptyWindowRect_return]
	NO_SHARED:
		mov eax, dword ptr [esp + 8]
		mov ecx, dword ptr [esi - 0xA8]
		mov dword ptr [eax], ecx
		mov dword ptr [eax + 8], ecx
		mov ecx, dword ptr [esi - 0xA4]
		mov dword ptr [eax + 4], ecx
		mov dword ptr [eax + 0xC], ecx
		pop esi
		ret 4
	}
}
