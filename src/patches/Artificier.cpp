#include "pch.h"

// CHANGE FORMULA FOR MINI-ARTIFACT STATS

void Artificier_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_WRTE, 0x00C0A302, 57, nullptr, 0, 0, 0, "83F8058974240474358BC80FAF348D0CA4C0008BC6BA000000008B350CA4C000F7FE408B348DB2A3C00039F00F8C020000008BC65E83C40CC3" });
	assembly_patches.push_back({ PATCH_NOP, 0x00C0A33B, 5, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_NOP, 0x00C0A3DA, 49, nullptr, 0, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_INT, 0x00C0A40C, 4, nullptr, 1000, 0, 0, 0 });

	assembly_patches.push_back({ PATCH_INT, 0x00C0A410, 4, nullptr, 200, 0, 0, 0 }); // attack ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A414, 4, nullptr, 200, 0, 0, 0 }); // break ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A418, 4, nullptr, 60, 0, 0, 0 }); // health ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A41C, 4, nullptr, 200, 0, 0, 0 }); // defense ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A424, 4, nullptr, 40, 0, 0, 0 }); // luck ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A428, 4, nullptr, 40, 0, 0, 0 }); // morale ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A42C, 4, nullptr, 20, 0, 0, 0 }); // speed ratio
	assembly_patches.push_back({ PATCH_INT, 0x00C0A430, 4, nullptr, 400, 0, 0, 0 }); // initiative ratio

	assembly_patches.push_back({ PATCH_INT, 0x00C0A3B6, 4, nullptr, 30, 0, 0, 0 }); // attack max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3BA, 4, nullptr, 30, 0, 0, 0 }); // break max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3BE, 4, nullptr, 10, 0, 0, 0 }); // health max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3C2, 4, nullptr, 30, 0, 0, 0 }); // defense max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3CA, 4, nullptr, 5, 0, 0, 0 }); // luck max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3CE, 4, nullptr, 5, 0, 0, 0 }); // morale max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3D2, 4, nullptr, 3, 0, 0, 0 }); // speed max
	assembly_patches.push_back({ PATCH_INT, 0x00C0A3D6, 4, nullptr, 60, 0, 0, 0 }); // initiative max
}
