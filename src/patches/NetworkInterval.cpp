#include "pch.h"

// Multiplayer: the network layer sends its pending data every 0.02 s instead of 0.05 s
// [edi+3E8E4h] is a countdown, decreased by the frame time at 0x0047E7E5; when it reaches 0 the data is sent and
// the countdown is reset to 0.05 (mov dword ptr [edi+3E8E4h],3D4CCCCDh at 0x0047E54D, float immediate at +6).
// All players of a multiplayer game must use the same DLL.

static const float network_send_interval = 0.02f;   // seconds, vanilla 0.05

int NetworkInterval_value = 0x0047E553;

void NetworkInterval_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT, NetworkInterval_value, 4, nullptr, 0, network_send_interval, 0 });
}
