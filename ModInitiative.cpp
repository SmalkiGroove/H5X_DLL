#include "pch.h"

// ADD INITIATIVE FROM ADDITIONAL ARTIFACTS
// (131) Plunderer Shoes : +1 creatures init
// (57)  Boots of Swiftness : +2 creatures init
// (157) Harpy Boots : +1 creatures init
// (22) Celestial Justicar Sword : +15% creature init
// (6) Staff of Netherworld : -15% enemy creatures init
// (143) Skull Mask : -8% enemy creatures init
// (58) Moon Blade : +1 hero init
// (107) Ring of Celerity : +2 hero init
// (65) Ring of Caution : -2 hero init
// (201/202/203) Artificier : 6% global init

void CreatureInitiativeFork();
void CreatureInitlessFork();
void HeroInitiativeFork();

int CreatureInitiative_fork = 0x004BC2BF;
int CreatureInitiative_return = 0x004BC2D8;
int CreatureInitless_fork = 0x008A4621;
int CreatureInitless_return = 0x008A464B;
int HeroInitiative_fork = 0x00BC1F77;
int HeroInitiative_return = 0x00BC1F7D;

void Initiative_init(pugi::xml_document& doc) {
    assembly_patches.push_back({ PATCH_HOOK, CreatureInitiative_fork, 25, CreatureInitiativeFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, CreatureInitless_fork, 42, CreatureInitlessFork, 0, 0, 0 });
    assembly_patches.push_back({ PATCH_HOOK, HeroInitiative_fork, 6, HeroInitiativeFork, 0, 0, 0 });
}

__declspec(naked) void CreatureInitiativeFork() {
    __asm
    {
        call dword ptr [edx + 0x74]
        mov edi, eax

        push 0x83
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_2
        fld dword ptr ss : [esp + 0x20]
        fadd dword ptr [constf_1]
        fstp dword ptr ss : [esp + 0x20]

        CRT_INIT_2:
        push 0x39
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_3
        fld dword ptr ss : [esp + 0x20]
        fadd dword ptr [constf_2]
        fstp dword ptr ss : [esp + 0x20]

        CRT_INIT_3:
        push 0x9D
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_PERCENT
        fld dword ptr ss : [esp + 0x20]
        fadd dword ptr [constf_1]
        fstp dword ptr ss : [esp + 0x20]

        CRT_INIT_PERCENT:
        push 0x16
        mov ecx, edi
        xor esi, esi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_A1
        mov esi, 0xF
            
        CRT_INIT_A1:
        push 0xC9
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_A2
        lea eax, dword ptr [eax + eax * 4]
        lea esi, dword ptr [esi + eax * 1]

        CRT_INIT_A2:
        push 0xCA
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_A3
        lea eax, dword ptr [eax + eax * 2]
        lea esi, dword ptr [esi + eax * 2]

        CRT_INIT_A3:
        push 0xCB
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je CRT_INIT_END
        mov ecx, eax
        lea ecx, dword ptr [eax + eax * 2]
        lea eax, dword ptr [eax + ecx * 2]
        lea esi, dword ptr [esi + eax]
        CRT_INIT_END:
        jmp[CreatureInitiative_return]
    }
}

float staff_of_netherworld_malus = 0.15f;
float skull_mask_malus = 0.08f;
__declspec(naked) void CreatureInitlessFork() {
    __asm
    {
        mov ecx, eax
        call dword ptr [edx + 0x74]
        mov edi, eax

        push 0x6
        mov ecx, edi
        call[count_equipped_artifact]
        mov dword ptr ss : [esp + 0x14], eax
        fild dword ptr ss : [esp + 0x14]
        fmul dword ptr [staff_of_netherworld_malus]
        fsubr dword ptr [constf_1]
        fmul dword ptr ss : [esp + 0x10]
        fstp dword ptr ss : [esp + 0x10]

        push 0x8F
        mov ecx, edi
        call[count_equipped_artifact]
        mov dword ptr ss : [esp + 0x14] , eax
        fild dword ptr ss : [esp + 0x14]
        fmul dword ptr [skull_mask_malus]
        fsubr dword ptr[constf_1]
        fmul dword ptr ss : [esp + 0x10]
        fstp dword ptr ss : [esp + 0x10]

        jmp[CreatureInitless_return]
    }
}

int sub_get_some_data = 0x00420DF0;
float knowledge_to_init_ratio = 0.005f;
__declspec(naked) void HeroInitiativeFork() {
    __asm
    {
        mov eax, dword ptr [esi - 0x118]
        mov ecx, dword ptr [eax + 0x8]
        lea esi, dword ptr [ecx + esi - 0x118]
        push edi
        lea ecx, dword ptr ss : [esp + 0x10]
        call[sub_get_some_data]
        mov edx, dword ptr [esi]
        mov ecx, esi
        mov edi, eax
        call dword ptr [edx + 0x23C]
        pop eax
        mov eax, edx
        mov ecx, [esi - 0x6C]
        call dword ptr [edx + 0x74]
        mov edi, eax

        push 0x3A
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_2
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [constf_1]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_2:
        push 0x6B
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_3
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [constf_2]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_3:
        push 0x41
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_A1
        fld dword ptr ss : [esp + 0x4]
        fsub dword ptr [constf_2]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_A1:
        push 0xC9
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_A2
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [constf_0_6]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_A2:
        push 0xCA
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_A3
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [constf_0_6]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_A3:
        push 0xCB
        mov ecx, edi
        call[count_equipped_artifact]
        test eax, eax
        je HERO_INIT_END
        fld dword ptr ss : [esp + 0x4]
        fadd dword ptr [constf_0_6]
        fstp dword ptr ss : [esp + 0x4]

        HERO_INIT_END:
        mov eax, dword ptr [esi - 0x118]
        mov ecx, dword ptr [eax + 0x8]
        lea esi, dword ptr [ecx + esi - 0x118]
        push edi
        lea ecx, dword ptr ss : [esp + 0x10]
        call[sub_get_some_data]
        mov edi, eax
        mov edx, dword ptr [esi]
        mov ecx, esi
        call dword ptr [edx + 0x238]
        pop ecx
        mov ecx, dword ptr ss : [esp]
        mov dword ptr ss : [esp], eax
        fild dword ptr ss : [esp]
        fmul dword ptr [knowledge_to_init_ratio]
        fadd dword ptr [constf_0_1]
        fadd dword ptr ss : [esp + 0x4]
        fstp dword ptr ss : [esp + 0x4]
        mov dword ptr ss : [esp], ecx

        mov ecx, dword ptr ss : [esp + 0xC]
        test ecx, ecx

        jmp[HeroInitiative_return]
    }
}
