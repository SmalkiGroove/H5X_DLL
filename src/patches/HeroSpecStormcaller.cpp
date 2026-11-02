#include "pch.h"
#include "types/HeroSpecs.h"

// Stormcaller adds bonus lightning damage per level

void StormcallerFork();

int Stormcaller_fork = 0x00976DD3;
int Stormcaller_return = 0x00976DD8;
int Stormcaller_end1 = 0x00976EE4;
int Stormcaller_end2 = 0x00976EED;
int Stormcaller_endsub = 0x0097717E;

void StormcallerTweak_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Stormcaller_fork, 5, StormcallerFork, 0, 0, 0 });
}

// Hero level of a Stormcaller caster, 0 otherwise
static int __fastcall stormcaller_level(ICombatHero* caster) {
	return caster->has_spec(HERO_SPEC_STORMCALLER) ? caster->hero_level() : 0;
}

// ecx = spell, esi = caster unit (may be 0). Lightning bolt (3) and chain lightning (7) only.
// The bonus damage computation below continues the game's own code (stack slots, FPU control word).
__declspec(naked) void StormcallerFork() {
	__asm
	{
        call [get_spell_id]
        cmp eax, 0x3
        je STORMCALLER_START
        cmp eax, 0x7
        jne STORMCALLER_RETURN

    STORMCALLER_START:
        test esi, esi
        je STORMCALLER_ENDSUB

        mov ecx, esi
        call stormcaller_level
        test eax, eax
        jz STORMCALLER_END
        mov dword ptr [esp + 0x14], eax
        fild word ptr [esp + 0x14]
        fstp dword ptr [esp + 0x14]
        call [get_defaultstats]
        fld dword ptr [esp + 0x14]
        push 0x0
        fmul dword ptr [eax + 0x850]
        push 0x0
        fnstcw word ptr [esp + 0x1C]
        push HERO_SPEC_STORMCALLER
        movzx eax, word ptr [esp + 0x20]
        fadd dword ptr ds : 0x00E0A7E4
        or ah, 0xC
        fimul dword ptr [esp + 0x1C]
        mov dword ptr [esp + 0x1C], eax
        fldcw word ptr [esp + 0x1C]
        fistp dword ptr [esp + 0x1C]
        fldcw word ptr [esp + 0x20]
        jmp [Stormcaller_end1]

    STORMCALLER_RETURN:
        jmp [Stormcaller_return]
    STORMCALLER_END:
        jmp [Stormcaller_end2]
    STORMCALLER_ENDSUB:
        jmp [Stormcaller_endsub]
	}
}
