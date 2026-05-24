#include "pch.h"
#include "ElementsVirtuoso.h"

struct ElementalVirtuosoState {
    IHero* heroPtr;
    int  lastElem;
    int  stacks;
};

static ElementalVirtuosoState elementalVirtuosoSlots[2] = { {nullptr, 0, 0}, {nullptr, 0, 0} };

static int elementalVirtuosoGetSlot(IHero* heroPtr) {
    if (elementalVirtuosoSlots[0].heroPtr == heroPtr) return 0;
    if (elementalVirtuosoSlots[1].heroPtr == heroPtr) return 1;
    if (elementalVirtuosoSlots[0].heroPtr == nullptr) { elementalVirtuosoSlots[0].heroPtr = heroPtr; return 0; }
    if (elementalVirtuosoSlots[1].heroPtr == nullptr) { elementalVirtuosoSlots[1].heroPtr = heroPtr; return 1; }
    return -1;
}

void __fastcall elementalVirtuoso_reset() {
    elementalVirtuosoSlots[0] = { nullptr, 0, 0 };
    elementalVirtuosoSlots[1] = { nullptr, 0, 0 };
}

int __fastcall elementVirtuoso_check(IHero* heroPtr, int spellElement) {
    int mastery = heroPtr->skill_mastery(PERK_ELEMENTS_VIRTUOSO);
	if (mastery == 0) return 0;
	int slot = elementalVirtuosoGetSlot(heroPtr);
    if (slot == -1) return 0;
    return elementalVirtuosoSlots[slot].lastElem == spellElement ? 0 : elementalVirtuosoSlots[slot].stacks;
}

void __fastcall elementVirtuoso_cast(IHero* heroPtr, int spellElement) {
    int mastery = heroPtr->skill_mastery(PERK_ELEMENTS_VIRTUOSO);
	if (mastery == 0) return;
	int slot = elementalVirtuosoGetSlot(heroPtr);
	if (slot == -1) return;
	if (elementalVirtuosoSlots[slot].lastElem != spellElement) {
		elementalVirtuosoSlots[slot].lastElem = spellElement;
		elementalVirtuosoSlots[slot].stacks++;
	}
}



void ElementsVirtuosoFork1();
void ElementsVirtuosoFork2();

int ElementsVirtuoso_fork1 = 0x0056FB73;
int ElementsVirtuoso_return1 = 0x0056FB79;
//int ElementsVirtuoso_fork2 = 0x00976F60;

void ElementsVirtuoso_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, ElementsVirtuoso_fork1, 6, ElementsVirtuosoFork1, 0, 0, 0 });
}

__declspec(naked) void ElementsVirtuosoFork1() {
    __asm
    {
		call[elementalVirtuoso_reset]
        mov al, byte ptr [esi + 0x4F0]
		jmp[ElementsVirtuoso_return1]
    }
}
