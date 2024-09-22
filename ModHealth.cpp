#include "pch.h"

// ADD BONUS HP FROM ADDITIONAL ARTIFACTS
// (128) Plunderer Vest : +1 HP
// (169) Ring of Supernatural Strength : +3 HP
// (156) Barbarian Cuirass : +3% HP
// (100) Regal Armour : +6% HP (rounded up)

void HealthFork();

int Health_fork = 0x004BC4F4;
int Health_return = 0x004BC4FA;

void Health_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, Health_fork, 6, HealthFork, 0, 0, 0 });
}

__declspec(naked) void HealthFork() {
    __asm
    {
        lea ebp, dword ptr [edi + eax * 2]           // vanilla ring of vitality

        mov eax, dword ptr [esi]                     // set 4 bytes at esi into eax for later function use
        push 0x80                                    // push artifact 128 to the stack (esp) for function call
        mov ecx, esi                                 // set hero struct pointer to ecx for function call
        call dword ptr [eax + 0x74]                  // get equipped artifacts list from hero struct
        mov ecx, eax                                 // set equipped artifacts list to ecx for function call
        call[count_equipped_artifact]                // call function CountEquippedArtifact(ECX:artifact_list, ESP:artifact_id)
        test al, al                                  // check if value al is 0 and set result in zf
        je HEALTH_BONUS_2                            // jump to label if zf is 0
        add ebp, 0x1                                 // increase hp bonus value by 1
    HEALTH_BONUS_2:
        mov eax, dword ptr [esi]                     // set 4 bytes at esi into eax for later function use
        push 0xA9                                    // push artifact 169 to the stack (esp) for function call
        mov ecx, esi                                 // set hero struct pointer to ecx for function call
        call dword ptr [eax + 0x74]                  // get equipped artifacts list from hero struct
        mov ecx, eax                                 // set equipped artifacts list to ecx for function call
        call[count_equipped_artifact]                // call function CountEquippedArtifact(ECX:hero, ESP:artifact_id)
        test al, al                                  // check if value al is 0 and set result in zf
        je HEALTH_BONUS_3                            // jump to label if zf is 0
        add ebp, 0x3                                 // increase hp bonus value by 3
    HEALTH_BONUS_3:
        mov eax, dword ptr [esi]                      // set 4 bytes at esi into eax for later function use
        push 0x9C                                    // push artifact 156 to the stack (esp) for function call
        mov ecx, esi                                 // set hero struct pointer to ecx for function call
        call dword ptr [eax + 0x74]                   // get equipped artifacts list from hero struct
        mov ecx, eax                                 // set equipped artifacts list to ecx for function call
        call[count_equipped_artifact]                // call function CountEquippedArtifact(ECX:hero, ESP:artifact_id)
        test al, al                                  // check if value al is 0 and set result in zf
        je HEALTH_BONUS_4                            // jump to label if zf is 0
        mov ecx, dword ptr [ebx + 0x1C]               // set creature id into ecx for function call
        call[get_creature_data]                      // call function GetCreatureData(ECX:creature_id)
        mov eax, dword ptr [eax + 0x64]               // set creature health from data array into eax
        shr eax, 0x5                                 // divide value by 32 rounded down
        add ebp, eax                                 // add this value to the current bonus hp value
    HEALTH_BONUS_4:
        mov eax, dword ptr [esi]                     // set 4 bytes at esi into eax for later function use
        push 0x64                                    // push artifact 100 to the stack (esp) for function call
        mov ecx, esi                                 // set hero struct pointer to ecx for function call
        call dword ptr [eax + 0x74]                  // get equipped artifacts list from hero struct
        mov ecx, eax                                 // set equipped artifacts list to ecx for function call
        call[count_equipped_artifact]                // call function CountEquippedArtifact(ECX:hero, ESP:artifact_id)
        test al, al                                  // check if value al is 0 and set result in zf
        je HEALTH_BONUS_END                          // jump to label if zf is 0
        mov ecx, dword ptr [ebx + 0x1C]              // set creature id into ecx for function call
        call[get_creature_data]                      // call function GetCreatureData(ECX:creature_id)
        mov eax, dword ptr [eax + 0x64]              // set creature health from data array into eax
        shr eax, 0x4                                 // divide value by 16 rounded down
        add eax, 0x1                                 // add 1 to the value
        add ebp, eax                                 // add this value to the current bonus hp value
        
    HEALTH_BONUS_END:
        mov ecx, dword ptr [ebx + 0x1C]              // set creature id into ecx for function call
        jmp[Health_return]                           // return to where we forked
    }
}
