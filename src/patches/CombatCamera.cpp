#include "pch.h"

// Combat camera: wider pitch range and closer zoom
// Camera limits set up by 0x007D3730:
// - pitch: immediates stored at 0x007D3883 ([esp+6Ch] top, [esp+70h] bottom, radians). Top -1.0471976 (60°) ->
//   -1.6 (looks straight down), bottom -0.61086524 (35°) -> -0.3 (closer to the ground). Yaw (±pi/2) unchanged.
// - zoom: min / max camera distance come from switches on the arena width + 4 (vtable +0x44 of [ecx+0Ch],
//   0x00AF14xx); each case loads a constant that is then doubled. The H5X field (14 wide, BattlefieldSize.cpp)
//   and the 14 x 14 sieges give 18, which shares its case with 20: min distance 14.4 (0x007D3600, func 0x007D35A0)
//   -> 8, max distance 28 (0x007D36A5, func 0x007D3650) unchanged. Other widths keep their vanilla distances.

static const float combat_camera_top_pitch = -1.6f;
static const float combat_camera_bottom_pitch = -0.3f;
static const float combat_camera_zoom_in = 8.0f;     // doubled by the game

int CombatCameraTopPitch_value = 0x007D3887;       // mov dword ptr [esp+6Ch],0BF860A92h
int CombatCameraBottomPitch_value = 0x007D3895;    // mov dword ptr [esp+70h],0BF1C61AAh
int CombatCameraZoomIn_operand = 0x007D3602;       // fld dword ptr ds:[00E44670h] (14.4)

void CombatCamera_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_FLOAT, CombatCameraTopPitch_value, 4, nullptr, 0, combat_camera_top_pitch, 0 });
	assembly_patches.push_back({ PATCH_FLOAT, CombatCameraBottomPitch_value, 4, nullptr, 0, combat_camera_bottom_pitch, 0 });
	assembly_patches.push_back({ PATCH_FLOAT_PTR, CombatCameraZoomIn_operand, 4, nullptr, 0, combat_camera_zoom_in, 0 });
}
