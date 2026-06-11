#include "pch.h"
#include "globals.h"

std::ofstream logs;
int logs_enabled = 1;
pugi::xml_document h5_stats;

// constants
int consti_1 = 1;
int consti_2 = 2;
float constf_1 = 1.0f;
float constf_2 = 2.0f;
float constf_0_1 = 0.1f;
float constf_0_15 = 0.15f;
float constf_0_2 = 0.2f;
float constf_0_25 = 0.25f;
float constf_0_3 = 0.3f;
float constf_0_4 = 0.4f;
float constf_0_5 = 0.5f;
float constf_0_6 = 0.6f;
float constf_0_7 = 0.7f;
float constf_0_75 = 0.75f;
float constf_0_8 = 0.8f;
float constf_0_85 = 0.85f;
float constf_0_9 = 0.9f;
float constf_1_1 = 1.1f;
float constf_1_15 = 1.15f;
float constf_1_2 = 1.2f;
float constf_1_3 = 1.3f;
float constf_1_5 = 1.5f;
float constf_percent = 0.01f;
float varf = 0.0f;

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

int* offsetPointer(int* p, int offset) {
    return (p + offset);
}
