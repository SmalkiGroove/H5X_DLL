#pragma once
#ifndef GLOBALS_H
#define GLOBALS_H
#include <fstream>
#include <sstream>

// define log globals
enum log_id {
    LOG_INFO = 0,
    LOG_WARN = 1,
    LOG_ERROR = 2
};

#define INFO "[Info] "
#define ERRROR "[Error] "
#define WARNING "[Warning] "

extern std::ofstream logs;
extern pugi::xml_document m55_stats;

void initLog();
void writeLog(const char* type, const std::string& message);

// global template function for changing values in memory - byte, integer, float

template <typename T> void assignValueToAddress(T* addrPtr, const T& value) {
    DWORD oldProtect;

    if (!VirtualProtect(addrPtr, sizeof(T), PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
        return;
    }
    *addrPtr = value;
    //writeLog(INFO, "Value patched to: " + std::to_string(value));
    //writeLog(INFO, "Address pointer: " + std::to_string(reinterpret_cast<uintptr_t>(addrPtr)));
}

template <typename T>
void assignHexStringToStruct(T& structure, const std::string& hexString) {
    uint8_t* structPtr = reinterpret_cast<uint8_t*>(&structure);
    size_t structSize = sizeof(structure);

    if (hexString.length() < (structSize * 2)) {
        throw std::runtime_error("Hex string is too short to fill the structure");
    }

    for (size_t i = 0; i < structSize; ++i) {
        std::string byteString = hexString.substr(i * 2, 2);
        uint8_t byte = static_cast<uint8_t>(std::stoi(byteString, nullptr, 16));
        std::memcpy(structPtr + i, &byte, sizeof(byte));
    }
}

// game functions

typedef int(__thiscall* GameFunction)(int*);

extern GameFunction get_hero_data;
extern GameFunction has_hero_spec;

extern GameFunction get_skill_id;
extern GameFunction get_skill_mastery;

extern GameFunction count_equipped_artifact;

extern GameFunction get_spell_id;
extern GameFunction get_spell_element;
extern GameFunction get_spell_school;
extern GameFunction is_spell_empowered;

extern GameFunction get_creature_data;

extern GameFunction call_unknown_1;
extern GameFunction call_unknown_2;

#endif