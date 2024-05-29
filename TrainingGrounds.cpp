#include "pch.h"

// REMOVE HERO CLASS AND SKILL REQUIREMENTS FOR TRAINING CREATURES IN HAVEN TOWNS

void TrainingGrounds_Towngate();
void TrainingGrounds_Garrison();

void TrainingGrounds_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_NOP, 0x00CCEBAC, 2, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_WRTE, 0x00CD384A, 1, nullptr, 0, 0, 0, "EB" });
	assembly_patches.push_back({ PATCH_CALL, 0x0061509E, 10, TrainingGrounds_Towngate, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, 0x006150D7, 13, TrainingGrounds_Garrison, 0, 0, 0 });
}

__declspec(naked) void TrainingGrounds_Towngate() {
	__asm
	{
		push esi
		mov esi, dword ptr ss : [esp + 0x20]
		cmp esi, 0xD
		jne LABEL_TG1
		mov bl, 0x1
		pop esi
		ret
	LABEL_TG1:
		test eax, eax
		je LABEL_TG2
		mov bl, 0x1
		pop esi
		ret
	LABEL_TG2:
		xor bl,bl
		pop esi
		ret
	}
}

__declspec(naked) void TrainingGrounds_Garrison() {
	__asm
	{
		push esi
		mov esi, dword ptr ss : [esp + 0x20]
		cmp esi, 0xD
		jne LABEL_TG1
		mov eax, 0x1
		pop esi
		ret
	LABEL_TG1:
		test eax, eax
		je LABEL_TG2
		mov eax, 0x1
		pop esi
		ret
	LABEL_TG2:
		xor eax, eax
		pop esi
		ret
	}
}
