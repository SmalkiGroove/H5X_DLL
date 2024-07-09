#include "pch.h"

// ADD SPELL IMMUNITIES FROM ADDITIONAL ARTIFACTS
// (184) Forgotten Ring : Immune to Word of Light (35)
// (180) Moonlight Band : Immune to Curse of the Netherworld (21)
// (123) Obsidian Armor : Immune to Armageddon (10)
// (172) Sanctuary's Crown : Immune to Hypnotize (20)
// (165) Shantiri Breastplate : Immune to Earthquake (41)
// (115) Cloak of the Saint : Immune to Plague (14)

void SpellImmuneForkEarthquake();
void SpellImmuneCaseEarthquake();

#pragma pack(push, 1)
struct HeroModifiers_Immunity_destinations {
    uint32_t spell_wasp_regen;
    uint32_t spell_phantom;
    uint32_t spell_animate_dead;
    uint32_t spell_resurrection;
    uint32_t spell_roar_fear;
    uint32_t spell_default;
    void (*spell_earthquake)();
};
#pragma pack(pop)
HeroModifiers_Immunity_destinations immunity_destinations;
int Addr_return_immunity = 0x0097CD61;
int Addr_end_immunity = 0x0097CB28;

void SpellImmune_init(pugi::xml_document& doc) {
    assignHexStringToStruct(immunity_destinations.spell_wasp_regen, "0xFFCC9700");
    assignHexStringToStruct(immunity_destinations.spell_phantom, "0x99CC9700");
    assignHexStringToStruct(immunity_destinations.spell_animate_dead, "0x74CC9700");
    assignHexStringToStruct(immunity_destinations.spell_resurrection, "0x60CC9700");
    assignHexStringToStruct(immunity_destinations.spell_roar_fear, "023CD9700");
    assignHexStringToStruct(immunity_destinations.spell_default, "0x6CCD9700");

    assembly_patches.push_back({ PATCH_HOOK, 0x0097CC59, 7, SpellImmuneForkEarthquake, 0, 0, 0 });
    immunity_destinations.spell_earthquake = &SpellImmuneCaseEarthquake;
}


__declspec(naked) void SpellImmuneForkEarthquake() {
    __asm
    {
        jmp dword ptr ds : [eax * 4 + immunity_destinations]
    }
}

__declspec(naked) void SpellImmuneCaseEarthquake() {
    __asm
    {
        // setup args for get_hero_data ?
        call[get_hero_data]
        mov esi, eax
        test esi, esi
        jz LABEL_END
        mov ecx, eax
        mov edx, [eax]
        call dword ptr [edx + 0x74]
        push 0xA5
        mov ecx, eax
        call[count_equipped_artifact]
        test eax, eax
        jmp[Addr_return_immunity]
        LABEL_END:
        jmp[Addr_end_immunity]
    }
}
