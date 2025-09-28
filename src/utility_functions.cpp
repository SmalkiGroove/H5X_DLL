////////////////////////////////////////////////////////////////////
/////// UTILITY FUNCTIONS //////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////
#include "pch.h"
#include "mainH5.h"
#include "libzippp.h"

/* True if file exists, otherwise false */
bool fexists(char* path) {
    FILE* file;
    if (fopen_s(&file, path, "r") == 0) {
        fclose(file);
        return true;
    }
    return false;
}

pugi::xml_document getXmlTree(const char* archive, const char* xml_source) {

    pugi::xml_document doc;
    if (!fexists(const_cast<char*>(archive))) {
        //writeLog(ERRROR, archive + std::string(" does not exist!"));
        return doc;
    }
    libzippp::ZipArchive data(archive);
    data.open(libzippp::ZipArchive::ReadOnly);
    libzippp::ZipEntry config = data.getEntry(xml_source);
    
    if (config.isNull()) {
        //writeLog(ERRROR, std::string("Failed to read") + archive + "/" + xml_source + "!");
        return doc;
    }
    doc.load_string(config.readAsText().c_str());
    data.close();
    return doc;
}

void assignByteToAddress(long addrPtr, const int* value) {
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(addrPtr), 1, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
        return;
    }
    *(uint8_t*)addrPtr = static_cast<uint8_t>(*value);
}

void JumpToFunction(void* targetAddress, void* destinationFunction, size_t len, int patch_type)
{
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(targetAddress), len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
    }

    // Calculate the relative offset between the target and destination functions
    intptr_t offset = reinterpret_cast<intptr_t>(destinationFunction) - reinterpret_cast<intptr_t>(targetAddress) - 5;

    // Write the assembly bytes for the jmp instruction
    unsigned char jmpBytes[] = { patch_type, 0x00, 0x00, 0x00, 0x00 };
    std::memset((char*)targetAddress, 0x90, len);
    std::memcpy(jmpBytes + 1, &offset, sizeof(offset));

    // Copy the assembly bytes to the target function
    std::memcpy(targetAddress, jmpBytes, sizeof(jmpBytes));

    DWORD unused;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(targetAddress), sizeof(len), oldProtect, &unused)) {
        writeLog(ERRROR, "Cannot revert the current protection settings!");
    }
}

void ConditionalJumpToFunction(void* targetAddress, void* destinationFunction, size_t len, unsigned short patch_type)
{
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(targetAddress), len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERROR, "Cannot enable the current protection settings!");
    }

    // Calculate the relative offset between the target and destination functions
    intptr_t offset = reinterpret_cast<intptr_t>(destinationFunction) - reinterpret_cast<intptr_t>(targetAddress) - 6;

    // Write the assembly bytes for the conditional jump instruction
    unsigned char jmpBytes[] = { 0x0F, static_cast<unsigned char>(patch_type), 0x00, 0x00, 0x00, 0x00 };
    std::memset((char*)targetAddress, 0x90, len);
    std::memcpy(jmpBytes + 2, &offset, sizeof(offset));

    // Copy the assembly bytes to the target function
    std::memcpy(targetAddress, jmpBytes, sizeof(jmpBytes));

    DWORD unused;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(targetAddress), sizeof(len), oldProtect, &unused)) {
        writeLog(ERROR, "Cannot revert the current protection settings!");
    }
}

void AssignNopToAddressRange(int* hookAddress, size_t len) {
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(hookAddress), len, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
    }
    for (size_t i = 0; i < len; i++) {
        //sscanf_s(patch.value_bytes + 2 * i, "%2hhx", &byteArray[i]);
        void* memoryAddress = reinterpret_cast<void*>(hookAddress);
        unsigned char* targetAddress = static_cast<unsigned char*>(memoryAddress);
        for (size_t i = 0; i < len; i++) {
            targetAddress[i] = 0x90;
        }
    }
}

double getDoubleValueFromXdb(pugi::xml_document& doc, const char* abilityName) {
    std::string xpath = std::string("/MMH55/") + abilityName;
    auto node = doc.select_node(xpath.c_str()).node().text().as_double();
    return node;
}

void assignFloatPtrToAddress(float** addrPtr, const float* value) {
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(addrPtr), 4, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
        return;
    }
    *addrPtr = const_cast<float*>(value);
}

void assignDoublePtrToAddress(double** addrPtr, const double* value) {
    DWORD oldProtect;
    if (!VirtualProtect(reinterpret_cast<LPVOID>(addrPtr), 8, PAGE_EXECUTE_READWRITE, &oldProtect)) {
        writeLog(ERRROR, "Cannot enable the current protection settings!");
        return;
    }
    *addrPtr = const_cast<double*>(value);
}

float getFloatValueFromXdb(pugi::xml_document& doc, const char* abilityName) {
    std::string xpath = std::string("/MMH55/") + abilityName;
    auto node = doc.select_node(xpath.c_str()).node().text().as_float();
    return node;
}

int getIntValueFromXdb(pugi::xml_document& doc, const char* abilityName) {
    std::string xpath = std::string("/MMH55/") + abilityName;
    auto node = doc.select_node(xpath.c_str()).node().text().as_int();
    return node;
}

std::string getStringValueFromXdb(pugi::xml_document& doc, const char* abilityName) {
    std::string xpath = std::string("/MMH55/") + abilityName;
    auto node = doc.select_node(xpath.c_str()).node().text().as_string();
    return node;
}