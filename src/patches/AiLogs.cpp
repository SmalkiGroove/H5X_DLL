#include "pch.h"

// ENABLE THE ADVENTURE MAP AI ACTION LOGS
// Developer switch left off in release builds: when set, every AI player writes its decisions to
// ../ai_#N.log (relative to bin, N = player number 1..8), recreated at each map start.
// Debug addition: destination candidates dropped by the builder because their value is not positive
// (never logged by the game) are written as "destination (dropped, value <= 0)" rows.

void AiDroppedCandidateFork();

static const char ai_dropped_tag[] = "dropped, value <= 0";

int AiLogs_enabled = 0x01027180;
int AiNeutralCandidate_func = 0x00A78340;   // 2nd acceptance test of a candidate (value not positive)
int AiDestinationLog_func = 0x00A7A0C0;     // writes a "destination (<tag>)" row

void AiLogs_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, AiLogs_enabled, 1, nullptr, 1, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, 0x00A7B480, 5, AiDroppedCandidateFork, 0, 0, 0 }); // call FUN_00A78340
}

// Called from the candidate builder FUN_00A7AF80 in place of FUN_00A78340 (ECX = entry, EDX unchanged).
// Caller frame B = esp + 4: entry [B+0x4C], value [B+0x54], hero main interface [B+0x48],
// hero [B+0xA0], builder 5th argument [B+0xA8]. Returns the original result.
__declspec(naked) void AiDroppedCandidateFork() {
	__asm
	{
		call [AiNeutralCandidate_func]
		test al, al
		jnz CANDIDATE_KEPT
		mov ecx, [AiLogs_enabled]
		cmp byte ptr [ecx], 0
		je CANDIDATE_DROPPED

		lea eax, ai_dropped_tag
		push eax
		push dword ptr [esp + 0xA8]          // hero
		push dword ptr [esp + 0xB4]          // builder 5th argument
		lea eax, [esp + 0x64]                // value
		lea ecx, [esp + 0x5C]                // entry
		mov edx, dword ptr [esp + 0x58]      // hero main interface
		call [AiDestinationLog_func]         // ret 0Ch

		CANDIDATE_DROPPED:
		xor eax, eax

		CANDIDATE_KEPT:
		ret
	}
}
