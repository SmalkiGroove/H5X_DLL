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

typedef int(__thiscall* GetHeroDataFunc)(int*);
extern GetHeroDataFunc get_hero_data;

typedef int(__thiscall* SkillIdFromStrFunc)(int* skill);
extern SkillIdFromStrFunc skill_id_from_str;

typedef int(__thiscall* ReturnMasteryIdFunc)(int* mastery);
extern ReturnMasteryIdFunc mastery_id_from_str;

typedef int(__thiscall* ReturnSpellIdFunc)(int*);
extern ReturnSpellIdFunc spell_id_from_str;
int spell_id_from_str_precursor(std::string spell_name);

typedef bool(__thiscall* IsSpellEmpoweredFunc)(int* spell);
extern IsSpellEmpoweredFunc is_spell_empowered;

typedef int(__thiscall* CountEquippedArtifactFunc)(int* artifact);
extern CountEquippedArtifactFunc count_equipped_artifact;

typedef int(__thiscall* GetCreatureDataFunc)(int* creature);
extern GetCreatureDataFunc get_creature_data;

typedef int(__thiscall* GetSpellElementFunc)(int* spell);
extern GetSpellElementFunc get_spell_element;

typedef int(__thiscall* Unknown1Func)(int*);
extern Unknown1Func call_unknown_1;

#endif