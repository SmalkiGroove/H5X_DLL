#include "pch.h"
// Fix Arcane renewal being triggered when Summon Hive, Blade Barrier and Arcane Crystal are cast succesfully

void ArcaneRenewalFix();

void ArcaneRenewalFix_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, 0x0097E83D, 5, ArcaneRenewalFix, 0 , 0 });
}

int ArcaneRenewalFix_summon_creature_or_spell_on_the_battlefield = 0x00C55A40;
int ArcaneRenewalFix_return = 0x0097E842;
__declspec(naked) void ArcaneRenewalFix() {
    __asm
    {
        call[ArcaneRenewalFix_summon_creature_or_spell_on_the_battlefield];
        mov byte ptr [esp + 0x13] , 0
            jmp[ArcaneRenewalFix_return];
    }
}