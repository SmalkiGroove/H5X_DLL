#include "pch.h"

// RMG TREASURE BLOCK ARTIFACT LOG (diagnostic)
// Logs in H5.log every artifact the random map generator tries to put in a treasure block, and whether the object was
// created, to find the guarded empty tiles. See .cursor/rules/h5x-re-rmg.mdc.
// func_TreasureBlockDistributor_Distribute (0x00BA94C0) draws one artifact per block and places it on the block's
// tile 1 with func_RMG_PlaceObject (call at 0x00BA9AEF, al = 1 when the object was created). A failure skips the tile
// without any log, after the block guard was already added to the map.
// At the hook (stack as in the function body): esi = CTreasureBlockDistributor, ebx = block offset in the block vector
// (+0x7C, 0x1C-byte records: tile vector begin +0x08, value +0x14), [esp+0x28] = candidate artifact IDs,
// [esp+0x54] = drawn index, [esp+0x34] = shared DB link passed to func_RMG_PlaceObject: { type string (begin, end,
// cap), byte, int, href string (begin +0x14, end +0x18) }.
// The logged value is the block value after the artifact's CostOfGold / 5 was taken out.

void RmgArtifactLogFork();

int RmgArtifactLog_fork = 0x00BA9AF4;     // test al,al / je 0x00BA9DA1
int RmgArtifactLog_return = 0x00BA9AFC;   // object created: add its link to the map list
int RmgArtifactLog_fail = 0x00BA9DA1;     // next block tile

void RmgArtifactLog_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, RmgArtifactLog_fork, 8, RmgArtifactLogFork, 0, 0, 0 });
}

typedef char*(__fastcall* GetArtifactInfoFn)(int);
static GetArtifactInfoFn get_artifact_info = (GetArtifactInfoFn)0x008481D0;   // func_GetArtifactInfo

static void __stdcall log_rmg_artifact(char* distributor, int block_offset, int artifact_id, char** link, int placed) {
	char* block = *(char**)(distributor + 0x7C) + block_offset;
	float* tiles = *(float**)(block + 0x08);
	char* info = get_artifact_info(artifact_id);
	std::string href = (link[5] && link[6] >= link[5]) ? std::string(link[5], link[6]) : std::string();
	writeLog(placed ? INFO : WARNING, std::string("RmgArtifact: ") + (placed ? "placed" : "NOT PLACED")
		+ " artifact " + std::to_string(artifact_id)
		+ " (class " + std::to_string(*(int*)(info + 0x24)) + ", cost " + std::to_string(*(int*)(info + 0x34)) + ")"
		+ " at " + std::to_string((int)tiles[2]) + ":" + std::to_string((int)tiles[3])
		+ " floor " + std::to_string(*(int*)(distributor + 0x2C))
		+ ", block value " + std::to_string(*(int*)(block + 0x14))
		+ ", shared \"" + href + "\"");
}

__declspec(naked) void RmgArtifactLogFork() {
	__asm
	{
		pushad
		movzx eax, al
		push eax                            // placed
		lea edx, [esp + 0x24 + 0x34]
		push edx                            // shared DB link
		mov ecx, dword ptr [esp + 0x28 + 0x28]
		mov edx, dword ptr [esp + 0x28 + 0x54]
		mov ecx, dword ptr [ecx + edx * 4]
		push ecx                            // artifact ID
		push ebx                            // block offset
		push esi                            // distributor
		call log_rmg_artifact
		popad
		test al, al
		je FAIL
		jmp [RmgArtifactLog_return]
	FAIL:
		jmp [RmgArtifactLog_fail]
	}
}
