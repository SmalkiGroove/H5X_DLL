#include "pch.h"

// ENABLE THE ADVENTURE MAP AI ACTION LOGS
// Developer switch left off in release builds: when set, every AI player writes its decisions to
// ../ai_#N.log (relative to bin, N = player number 1..8), recreated at each map start.

void AiLogs_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_BYTE, 0x01027180, 1, nullptr, 1, 0, 0, 0 });
}
