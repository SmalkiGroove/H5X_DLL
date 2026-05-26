#include "pch.h"
#include "ElementsVirtuoso.h"

struct ElementalVirtuosoState {
    int* heroPtr;
    bool elem[5];
};

static ElementalVirtuosoState elementalVirtuosoSlots[2] = {
    {nullptr, {false, false, false, false, false}},
    {nullptr, {false, false, false, false, false}}
};

static int elementalVirtuosoGetSlot(int* heroPtr) {
    if (elementalVirtuosoSlots[0].heroPtr == heroPtr) return 0;
    else if (elementalVirtuosoSlots[1].heroPtr == heroPtr) return 1;
    else if (elementalVirtuosoSlots[0].heroPtr == nullptr) elementalVirtuosoSlots[0].heroPtr = heroPtr;
    else if (elementalVirtuosoSlots[1].heroPtr == nullptr) elementalVirtuosoSlots[1].heroPtr = heroPtr;
    return -1;
}

void __stdcall elementalVirtuoso_reset() {
    elementalVirtuosoSlots[0] = { nullptr, {false, false, false, false, false} };
    elementalVirtuosoSlots[1] = { nullptr, {false, false, false, false, false} };
}

int __fastcall elementVirtuoso_check(int* heroPtr) {
    int stacks = 0;
    if (has_skill(heroPtr, PERK_ELEMENTS_VIRTUOSO) == 1) {
        int slot = elementalVirtuosoGetSlot(heroPtr);
        if (slot != -1) {
            for (int i = 1; i < 5; i++) {
                if (elementalVirtuosoSlots[slot].elem[i]) stacks++;
            }
        }
    }
    return stacks;
}

void __fastcall elementVirtuoso_cast(int* heroPtr, int spellElement) {
    if (spellElement != 0) {
        if (has_skill(heroPtr, PERK_ELEMENTS_VIRTUOSO) == 1) {
            int slot = elementalVirtuosoGetSlot(heroPtr);
            if (slot != -1) {
                elementalVirtuosoSlots[slot].elem[spellElement] = true;
            }
        }
	}
}



void ElementsVirtuosoFork1();
void ElementsVirtuosoFork2();

int ElementsVirtuoso_fork1 = 0x004D8C36;
int ElementsVirtuoso_return1 = 0x004D8C3B;
int ElementsVirtuoso_fork2 = 0x0097E89F;
int ElementsVirtuoso_return2 = 0x0097E8A4;

void ElementsVirtuoso_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ElementsVirtuoso_fork1, 5, ElementsVirtuosoFork1, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, ElementsVirtuoso_fork2, 5, ElementsVirtuosoFork2, 0, 0, 0 });
}

__declspec(naked) void ElementsVirtuosoFork1() {
    __asm
    {
        pushad
        call elementalVirtuoso_reset
        popad
        mov eax, dword ptr [esi]
        call dword ptr [eax + 0xC]
		jmp[ElementsVirtuoso_return1]
    }
}

__declspec(naked) void ElementsVirtuosoFork2() {
    __asm
    {
        mov eax, dword ptr ss: [esp + 0x28]
        mov ecx, dword ptr [eax + 0x4]
        call[get_spell_id]
        mov ecx, eax
        call[get_spell_element]
        mov edx, eax
        mov ecx, esi
        call elementVirtuoso_cast

        pop edi
        pop esi
        mov eax, ebp
        pop ebp
        jmp[ElementsVirtuoso_return2]
    }
}
