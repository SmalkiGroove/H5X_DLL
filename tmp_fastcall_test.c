#include <stdio.h>
#include <stddef.h>
#include <stdint.h>
#pragma pack(push, 1)
struct CombatScriptParseFrame {
    char _pad[0x18];
    int parseField0;
    int parsedNameObject;
    int _field20;
    float parsedValue;
    int _field28;
    char valueTypeDesc[12];
    char nameTypeDesc[12];
};
#pragma pack(pop)
char __fastcall probe(void* a, int b, float* c, void* d) {
    uint32_t esp_val;
    __asm { mov esp_val, esp }
    printf("esp=%08X ecx=%p edx=%d c=%p d=%p\n", esp_val, a, b, c, d);
    uint32_t s4, s8;
    __asm {
        mov eax, esp
        mov s4, [eax+4]
        mov s8, [eax+8]
    }
    printf("stack+4=%08X stack+8=%08X\n", s4, s8);
    return 1;
}
int main() {
    CombatScriptParseFrame f = {};
    printf("size=%zu off18=%zu off24=%zu off2c=%zu off30=%zu\n",
        sizeof(f), offsetof(CombatScriptParseFrame,parseField0),
        offsetof(CombatScriptParseFrame,parsedValue),
        offsetof(CombatScriptParseFrame,valueTypeDesc),
        offsetof(CombatScriptParseFrame,valueTypeDesc)+4);
    probe(&f.parseField0, 42, &f.parsedValue, f.valueTypeDesc+4);
    return 0;
}
