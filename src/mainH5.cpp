#include "pch.h"
#include "mainH5.h"
#include <iostream>
#include <fstream>

std::vector<assembly_patch> assembly_patches;

void init_patches(pugi::xml_document& doc) {

    Scripts_init(doc);                     // OK
    Pest_init(doc);                        // OK
    HeroMaxSkills_init(doc);               // OK
    HeroScreenUI_init(doc);                // OK
    BattlefieldSize_init(doc);             // OK
    
    ArcaneRenewalFix_init(doc);            // OK
    EmpoweredArmageddon_init(doc);         // OK
    EnlightenmentForBarbsFix_init(doc);    // OK
    ImbueBalista_init(doc);                // OK
    SnareFix_init(doc);                    // OK
    RuneOfTheDragonForm_init(doc);         // OK
    CombatAIFix_init(doc);                 // OK
    AgilityFix_init(doc);                  // OK
    RetaliationStrikeFix_init(doc);        // OK
    ColdDeathFix_init(doc);                // OK
    EliteCasters_init(doc);                // OK
    UnholyWordImmunitiesFix_init(doc);     // OK
    BookOfPowerFix_init(doc);              // OK
    MasterOfFireFix_init(doc);             // OK
    EncourageFix_init(doc);                // OK

    Health_init(doc);                      // OK
    //CreatureAttack_init(doc);              //
    //CreatureDefense_init(doc);             //
    DamageOutput_init(doc);                // OK
    Initiative_init(doc);                  // OK
    Speed_init(doc);                       // OK
    Morale_init(doc);                      // OK
    Luck_init(doc);                        // OK
    ElemProof_init(doc);                   // OK
    MagicProof_init(doc);                  // OK
    MagicResist_init(doc);                 // OK
    SpellImmune_init(doc);                 //
    ManaCost_init(doc);                    // OK
    ElemDamage_init(doc);                  // OK
	CreatureAbilities_init(doc);           // OK

    HeroMovement_init(doc);                // OK
    CovenMistressTweak_init(doc);          // OK
    MasterOfElementsTweak_init(doc);       // OK
    EmbalmerTweak_init(doc);               // OK
    StormcallerTweak_init(doc);            // OK
    OverheaterTweak_init(doc);             // OK
    ElementalistChain_init(doc);           // OK
    TemptressTweak_init(doc);              // OK
    InfantryCommanderTweak_init(doc);      // OK
    Enlightened_init(doc);                 // OK
    ShamanMatriarchTweak_init(doc);        // OK
    LoneHunterTweak_init(doc);             // OK
    ExhausterTweak_init(doc);              // OK
    WorldBreaker_init(doc);                // OK
	SpecBlackKnights_init(doc);            // OK

    AtbTweaks_init(doc);                   // OK
    CreatureSpellpower_init(doc);          // OK
    BattleDiveTweak_init(doc);             // OK
    EnergyChannelTweak_init(doc);          // OK
    PawStrikeTweak_init(doc);              // OK
    WhipStrikeTweak_init(doc);             // OK
    FirstAidTent_init(doc);                // OK
    BallistaCatapult_init(doc);            // OK
    TowerDamage_init(doc);                 // OK
    Fortunate_init(doc);                   // OK
    //Magnetism_init(doc);                   //

    Avengers_init(doc);                    // OK
    Artificier_init(doc);                  // OK
    Arcanism_init(doc);                    // OK
    CombatSkill_init(doc);                 // OK
    ElementalVision_init(doc);             // OK
    EnlightenmentStats_init(doc);          // OK
    ElementalWarriors_init(doc);           // OK
    OffDefFormation_init(doc);             // OK
    ChillingBones_init(doc);               // OK
    ErraticMana_init(doc);                 // OK
    WeakeningStrike_init(doc);             // OK
    SkillEffectDisable_init(doc);          // OK
    VengefulLight_init(doc);               // OK
	ExaltationTweak_init(doc);			   // 
    TwilightTweak_init(doc);               // OK
    PariahTweak_init(doc);                 // OK
    ArcaneBrillance_init(doc);             // OK
    ThunderPower_init(doc);                // OK
    BarbarianSkillsLinks_init(doc);        // OK

    GoldenGoose_init(doc);                 // OK
    WandOfSpellTweak_init(doc);            //
    AllSeeingCrownTweak_init(doc);         // OK
	//ShieldOfCrystalIce_init(doc);          //
	PendantOfMastery_init(doc);            // OK

    LightningStun_init(doc);               // OK
    ChainLightning_init(doc);              // OK
    Earthquake_init(doc);                  // 
    Vulnerability_init(doc);               // OK
    Frenzy_init(doc);                      // OK
    //ConjureAtb_init(doc);                  //
    
    BaseMorale_init(doc);                  // OK
    SplitStack_init(doc);                  // OK
    TrainingGrounds_init(doc);             // OK
    BarbarianShrineExp_init(doc);          // OK
    WarMachineTier_init(doc);              // OK
    LearnWarcries_init(doc);               // OK
    GiveHeroSkill_init(doc);               // OK
    HallOfIntrigue_init(doc);              // OK
    
}

int main() {
    initLog();
    writeLog(INFO, "Opening game files...");

    // ########### Get mod settings
    pugi::xml_document h5_stats;

    // ########### Apply assembly patches
    writeLog(INFO, "Initialize patches");
    init_patches(h5_stats);
    writeLog(INFO, "Commence patching...");
    for (const auto& patch : assembly_patches) {
        unsigned char* byteArray = new unsigned char[128];
        switch (patch.type) {
        case PATCH_HOOK:
            JumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0xE9);
            break;
        case HOOK_JE:
            ConditionalJumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0x84);
            break;
        case HOOK_JNE:
            ConditionalJumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0x85);
            break;
        case PATCH_BYTE:
            assignByteToAddress(patch.address, &patch.value_int);
            break;
        case PATCH_INT:
            assignValueToAddress((int*)patch.address, patch.value_int);
            break;
        case PATCH_NOP:
            AssignNopToAddressRange((int*)patch.address, patch.size);
            break;
        case PATCH_FLOAT:
            assignValueToAddress((float*)patch.address, patch.value_float);
            break;
        case PATCH_FLOAT_PTR:
            assignFloatPtrToAddress(reinterpret_cast<float**>(patch.address), &patch.value_float);
            break;
        case PATCH_CALL:
            JumpToFunction((void*)patch.address, patch.hookedFunction, patch.size, 0xE8);
            break;
        case PATCH_DOUBLE_PTR:
            assignDoublePtrToAddress(reinterpret_cast<double**>(patch.address), &patch.value_double);
            break;
        case PATCH_WRTE:
            DWORD oldProtect;
            if (!VirtualProtect(reinterpret_cast<LPVOID>(patch.address), patch.size, PAGE_EXECUTE_READWRITE, &oldProtect)) {
                writeLog(ERRROR, "Cannot enable the current protection settings!");
                return false;
            }
            for (size_t i = 0; i < patch.size; i++) {
                sscanf_s(patch.value_bytes + 2 * i, "%2hhx", &byteArray[i]);
                void* memoryAddress = reinterpret_cast<void*>(patch.address);
                unsigned char* targetAddress = static_cast<unsigned char*>(memoryAddress);
                for (size_t i = 0; i < patch.size; i++) {
                    targetAddress[i] = byteArray[i];
                }
            }
            break;
        default:
            writeLog(ERRROR, " Unsupported patch type!");
            break;
        }
        delete[] byteArray;
    }
    writeLog(INFO, "Patching completeled!");
}