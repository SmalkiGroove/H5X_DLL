#pragma once
#include "pugixml.hpp"
#include <vector>

int main();

// utility functions
bool fexists(char* path);
pugi::xml_document getXmlTree(const char* archive, const char* xml_source);
void assignByteToAddress(long addrPtr, const int* value);
void assignFloatPtrToAddress(float** addrPtr, const float* value);
void assignDoublePtrToAddress(double** addrPtr, const double* value);
void JumpToFunction(void* targetAddress, void* destinationFunction, int len, int patch_type);
void ConditionalJumpToFunction(void* targetAddress, void* destinationFunction, int len, unsigned short patch_type);
void AssignNopToAddressRange(int* hookAddress, int len);
double getDoubleValueFromXdb(pugi::xml_document& doc, const char* abilityName);
float getFloatValueFromXdb(pugi::xml_document& doc, const char* abilityName);
int getIntValueFromXdb(pugi::xml_document& doc, const char* abilityName);
std::string getStringValueFromXdb(pugi::xml_document& doc, const char* abilityName);

enum patch_type {
    PATCH_HOOK = 0,
    PATCH_BYTE = 1,
    PATCH_INT = 2,
    PATCH_FLOAT = 3,
    PATCH_FLOAT_PTR = 4,
    PATCH_CALL = 5,
    PATCH_WRTE = 6,
    HOOK_JE = 7,
    HOOK_JNE = 8,
    PATCH_DOUBLE = 9,
    PATCH_DOUBLE_PTR = 10,
    PATCH_NOP = 11,
};

struct assembly_patch {
    patch_type type;
    long address;
    size_t size;
    void* hookedFunction;
    int value_int;
    float value_float;
    double value_double;
    char value_bytes[100];
};

extern std::vector<assembly_patch> assembly_patches;


// patches

void Scripts_init(pugi::xml_document& doc);
void Pest_init(pugi::xml_document& doc);
void HeroMaxSkills_init(pugi::xml_document& doc);
void HeroScreenUI_init(pugi::xml_document& doc);
void BattlefieldSize_init(pugi::xml_document& doc);

void ArcaneRenewalFix_init(pugi::xml_document& doc);
void EmpoweredArmageddon_init(pugi::xml_document& doc);
void EnlightenmentForBarbsFix_init(pugi::xml_document& doc);
void ImbueBalista_init(pugi::xml_document& doc);
void SnareFix_init(pugi::xml_document& doc);
void RuneOfTheDragonForm_init(pugi::xml_document& doc);
void CombatAIFix_init(pugi::xml_document& doc);
void AgilityFix_init(pugi::xml_document& doc);
void RetaliationStrikeFix_init(pugi::xml_document& doc);

void AtbTweaks_init(pugi::xml_document& doc);
void CreatureSpellpower_init(pugi::xml_document& doc);
void BattleDiveTweak_init(pugi::xml_document& doc);
void EnergyChannelTweak_init(pugi::xml_document& doc);
void PawStrikeTweak_init(pugi::xml_document& doc);
void FirstAidTent_init(pugi::xml_document& doc);
void BallistaCatapult_init(pugi::xml_document& doc);

void Avengers_init(pugi::xml_document& doc);
void Arcanism_init(pugi::xml_document& doc);
void EnlightenmentStats_init(pugi::xml_document& doc);
void ElementalWarriors_init(pugi::xml_document& doc);
void OffDefFormation_init(pugi::xml_document& doc);
void ChillingBones_init(pugi::xml_document& doc);
void ErraticMana_init(pugi::xml_document& doc);

void Health_init(pugi::xml_document& doc);
void Morale_init(pugi::xml_document& doc);
void Luck_init(pugi::xml_document& doc);

void BaseMorale_init(pugi::xml_document& doc);
void SplitStack_init(pugi::xml_document& doc);
void TrainingGrounds_init(pugi::xml_document& doc);
