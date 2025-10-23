#include "pch.h"

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


__declspec(naked) void IceProofAbilityFork() {
    __asm
    {
        mov edx, [ebx]
		mov ecx, ebx
		push 0x53
		call dword ptr [edx + 0x190]
		test eax, eax
		je OVERHEATER_RETURN
		mov edx, [ebx]
		mov ecx, ebx
		call dword ptr [edx + 0x20]
		push 0x0
		mov dword ptr [esp], eax
		fild dword ptr [esp]
		fmul dword ptr [constf_percent]
		fsubr dword ptr [constf_1]
		fstp dword ptr [esp]
		fld dword ptr [esp + 0x4C]
        fmul dword ptr [esp]
        fstp dword ptr [esp + 0x4C]
		pop eax

		OVERHEATER_RETURN:
		mov edx, [ebx]
		mov ecx, ebx
		call dword ptr [edx + 0x74]
		jmp[IceProofAbility_return]
    }
}
