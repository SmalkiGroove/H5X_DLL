#include "pch.h"
#include "globals.h"

std::ofstream logs;
int logs_enabled = 0;
pugi::xml_document h5_stats;

// game funcs
GameFunction get_hero_data = (GameFunction)0x00977700;
GameFunction has_hero_spec = (GameFunction)0x00B4B5A0;
GameFunction get_skill_id = (GameFunction)0x009F5C90;
GameFunction get_skill_mastery = (GameFunction)0x00ACFEF0;
GameFunction count_equipped_artifact = (GameFunction)0x00CA6680;
GameFunction get_spell_id = (GameFunction)0x00A34110;
GameFunction get_spell_element = (GameFunction)0x00A342C0;
GameFunction get_spell_school = (GameFunction)0x00A34350;
GameFunction is_spell_empowered = (GameFunction)0x00A34100;
GameFunction get_creature_data = (GameFunction)0x0089FBF0;
GameFunction notify_skill_buff = (GameFunction)0x009BB960;
GameFunction notify_artifact_buff = (GameFunction)0x009BB9C0;
GameFunction call_unknown_2 = (GameFunction)0x00BCB3F0;

// constants
int consti_1 = 1;
int consti_2 = 2;
float constf_1 = 1.0f;
float constf_2 = 2.0f;
float constf_0_1 = 0.1f;
float constf_0_2 = 0.2f;
float constf_0_3 = 0.3f;
float constf_0_4 = 0.4f;
float constf_0_5 = 0.5f;
float constf_0_6 = 0.6f;
float constf_0_7 = 0.7f;
float constf_0_8 = 0.8f;
float constf_0_9 = 0.9f;
float constf_1_1 = 1.1f;
float constf_1_2 = 1.2f;
float constf_1_3 = 1.3f;
float constf_1_5 = 1.5f;
float constf_percent = 0.01f;

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
