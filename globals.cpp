#include "pch.h"
#include "globals.h"

std::ofstream logs;
int logs_enabled = 0;
pugi::xml_document h5_stats;

// game funcs
SkillIdFromStrFunc skill_id_from_str = (SkillIdFromStrFunc)0x009F5C90;
ReturnMasteryIdFunc mastery_id_from_str = (ReturnMasteryIdFunc)0x00ACFEF0;
ReturnSpellIdFunc spell_id_from_str = (ReturnSpellIdFunc)0x009F29C0;
IsSpellEmpoweredFunc is_spell_empowered = (IsSpellEmpoweredFunc)0x00A34100;
CountEquippedArtifactFunc count_equipped_artifact = (CountEquippedArtifactFunc)0x00CA6680;
GetCreatureDataFunc get_creature_data = (GetCreatureDataFunc)0x0089FBF0;
GetSpellElementFunc get_spell_element = (GetSpellElementFunc)0x00A342C0;
Unknown1Func call_unknown_1 = (Unknown1Func)0x009BB9C0;

void initLog() {
    if (logs_enabled) {
        logs.open("H5.log");
        logs.close();
    }
}

void writeLog(const char* type, const std::string& message) {
    if (logs_enabled) {
        logs.open("H5.log", std::ios::app);
        if (logs.is_open()) {
            logs << type << message << std::endl;
            logs.close();
        }
    }
}

int spell_id_from_str_precursor(std::string spell_name)
{
    int spell_address[2];
    spell_address[0] = reinterpret_cast<int>(&spell_name[0]);
    spell_address[1] = spell_address[0] + spell_name.size();
    int spell_id = spell_id_from_str(spell_address);
    return spell_id;
}