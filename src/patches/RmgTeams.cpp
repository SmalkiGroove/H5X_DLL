#include "pch.h"
#include <algorithm>
#include <map>
#include "libzippp.h"

// RMG TEAMS
// Random map templates can fix the teams. The mod's template generator (heroes-expansion, tools/generators) ends an
// RMGTemplate .xdb with <Teams>: one Item per player count, with <Players> and <Team> (one team per start zone, start
// zones = CanBePlayerStart zones in Index order, teams numbered from 1). Vanilla skips the unknown element. A map
// generated from that template with a player count that has an entry gets CustomTeams and each player's Team, like a
// hand-made map; without an entry the map stays vanilla (no CustomTeams, every player alone).
// 1. Read: hook at the end of the RMGTemplate serializer FUN_004a2340 (esi = IXmlSaver, edi = template), read with the
//    same saver calls as the vanilla serializers: +0x48 IsReading, +0x50 StartChunk(name, index), +0x54 FinishChunk,
//    +0x1C child count, int reader 0x004149B0(saver, name, index, int*, int*) (int lists: "Item" with index 1..n, as
//    0x004A4880). Kept per template Name (+0x50, string begin / end).
//    The DLL only patches on the first TrackingCount call: a template loaded before that never reaches the hook (first
//    in-game test, 2026-10-27). Then the save step reads <Teams> itself from the template's .xdb (href of the resource's
//    DB link, +0x0C, string at +0x20 / +0x24) in the game paks (data\*.pak, UserMODs\*.h5u; the newest file wins when
//    several have it).
// 2. Apply: func_RMG_SaveMap calls FUN_00909ce0 (ecx = generator) at 0x0090BD1C, which writes the map's teams count from
//    SAdvMapDesc CustomTeams (+0x100) and players (+0x104, stride 0xD4, ActivePlayer +0x14, Team +0x18). Before it, the
//    hook ranks the template start zones by Index, finds each one's game zone (Index -> zone hash map per floor) and its
//    player number (zone +0xF0, 1-based, 0 = no player; player n is map players[n-1], see 0x00907397), and writes
//    CustomTeams and players.[i].Team (template team - 1, -1 for inactive players) through the map's editor property
//    interface, like func_RMG_PlaceTowns writes players.[i].ActivePlayer, and in the SAdvMapDesc struct.
// H5.log: "RmgTeams: ..." for every template read with teams and every generated map. See .cursor/rules/h5x-re-rmg.mdc.

int RmgTeamsRead_fork = 0x004A2465;     // add esp,3Ch / pop edi / xor eax,eax (end of FUN_004a2340)
int RmgTeamsRead_return = 0x004A246B;   // pop esi / ret 4
int RmgTeamsSave_fork = 0x0090BD1C;     // call 0x00909CE0 in func_RMG_SaveMap

typedef char(__fastcall* SaverStartChunkFn)(int* saver, int edx, const char* name, int index);
typedef char(__fastcall* SaverBoolFn)(int* saver, int edx);
typedef void(__fastcall* SaverVoidFn)(int* saver, int edx);
typedef int(__fastcall* SaverIntFn)(int* saver, int edx);
typedef void(__cdecl* SaverReadIntFn)(int* saver, const char* name, int index, int* value, int* value2);
typedef int(__fastcall* SaveMapTeamsFn)(int generator);
typedef int*(__fastcall* GetTemplateFn)(int* link);
typedef int*(__fastcall* GetResourceFn)(int* path);
typedef void*(__cdecl* DynamicCastFn)(void* ptr, int offset, void* src_type, void* dst_type, int is_reference);
typedef int*(__fastcall* GetEditorObjectFn)(int* db, int edx, int* path);
typedef char(__fastcall* SetPropertyFn)(int* object, int edx, char** path, int* value);
typedef void(__fastcall* MakeStringFn)(char** str, int edx, const char* begin, const char* end);
typedef void(__cdecl* EngineFreeFn)(void* ptr);
typedef void(__fastcall* ReleaseFn)(int* object);

static SaverReadIntFn saver_read_int = (SaverReadIntFn)0x004149B0;
static SaveMapTeamsFn save_map_teams = (SaveMapTeamsFn)0x00909CE0;
static GetTemplateFn get_template = (GetTemplateFn)0x004A69D0;
static GetResourceFn get_resource = (GetResourceFn)0x009B0F60;
static DynamicCastFn dynamic_cast_fn = (DynamicCastFn)0x00D119FC;
static MakeStringFn make_string = (MakeStringFn)0x00401B20;
static EngineFreeFn engine_free = (EngineFreeFn)0x00879240;
static ReleaseFn release_object = (ReleaseFn)0x008C6150;
static int* editor_db = (int*)0x0103FFE8;          // returned by 0x00BA06C0
static void* rtti_resource = (void*)0x00F27454;    // NDb::CResource
static void* rtti_adv_map_desc = (void*)0x00F29074; // NDb::SAdvMapDesc

// template Name -> player count -> team of each start zone (template numbering, from 1); a template read without
// <Teams> has an empty entry, a template never read has none
static std::map<std::string, std::map<int, std::vector<int>>> template_teams;

void RmgTeamsReadFork();
int __fastcall rmg_save_teams(int generator);

void RmgTeams_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_HOOK, RmgTeamsRead_fork, 6, RmgTeamsReadFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_CALL, RmgTeamsSave_fork, 5, (void*)rmg_save_teams, 0, 0, 0 });
}

static char saver_start(int* saver, const char* name, int index) {
	return ((SaverStartChunkFn)(*(int**)saver)[0x50 / 4])(saver, 0, name, index);
}

static void saver_finish(int* saver) {
	((SaverVoidFn)(*(int**)saver)[0x54 / 4])(saver, 0);
}

static int saver_count(int* saver) {
	return ((SaverIntFn)(*(int**)saver)[0x1C / 4])(saver, 0);
}

static std::string template_name(char* tmpl) {
	char** name = (char**)(tmpl + 0x50);
	return (name[0] && name[1] > name[0]) ? std::string(name[0], name[1]) : std::string();
}

static std::string teams_text(const std::vector<int>& teams) {
	std::string text;
	for (size_t i = 0; i < teams.size(); ++i) text += (i ? ", " : "") + std::to_string(teams[i]);
	return text;
}

static void __fastcall rmg_read_teams(char* tmpl, int* saver) {
	char reading = ((SaverBoolFn)(*(int**)saver)[0x48 / 4])(saver, 0);
	std::string name = template_name(tmpl);
	char has_teams = reading ? saver_start(saver, "Teams", 0) : 0;
	writeLog(INFO, "RmgTeams: read hook, template \"" + name + "\", reading " + std::to_string((int)reading)
		+ ", Teams " + std::to_string((int)has_teams));
	if (!reading) return;
	std::map<int, std::vector<int>>& entries = template_teams[name];
	entries.clear();
	if (!has_teams) return;
	int count = saver_count(saver);
	for (int i = 1; i <= count; ++i) {
		if (!saver_start(saver, "Item", i)) continue;
		int players = 0;
		saver_read_int(saver, "Players", 0, &players, &players);
		std::vector<int> teams;
		if (saver_start(saver, "Team", 0)) {
			int zones = saver_count(saver);
			teams.resize(zones, 0);
			for (int j = 1; j <= zones; ++j) saver_read_int(saver, "Item", j, &teams[j - 1], &teams[j - 1]);
			saver_finish(saver);
		}
		saver_finish(saver);
		entries[players] = teams;
		writeLog(INFO, "RmgTeams: template \"" + name + "\", " + std::to_string(players) + " players: teams "
			+ teams_text(teams));
	}
	saver_finish(saver);
}

__declspec(naked) void RmgTeamsReadFork() {
	__asm
	{
		add esp, 0x3C
		pushad
		mov edx, esi
		mov ecx, edi
		call rmg_read_teams
		popad
		pop edi
		xor eax, eax
		jmp [RmgTeamsRead_return]
	}
}

// Game zone of a template zone Index: per floor (CRandomMap +0x34, stride 0x120) a hash map at +0xA8 (buckets begin
// +0xAC / end +0xB0, nodes { next, Index, zone }), as func_RMG_PlaceTowns looks them up.
static char* find_game_zone(char* rmap, int index) {
	char* floors = *(char**)(rmap + 0x34);
	int floor_count = (*(char**)(rmap + 0x38) - floors) / 0x120;
	for (int f = 0; f < floor_count; ++f) {
		int** buckets = *(int***)(floors + f * 0x120 + 0xAC);
		int bucket_count = *(int***)(floors + f * 0x120 + 0xB0) - buckets;
		if (bucket_count <= 0) continue;
		for (int* node = buckets[(unsigned)index % (unsigned)bucket_count]; node; node = (int*)node[0])
			if (node[1] == index) return (char*)node[2];
	}
	return nullptr;
}

static bool set_property(int* object, const char* path, int type, int value) {
	char* str[3] = { nullptr, nullptr, nullptr };
	make_string(str, 0, path, path + strlen(path));
	int prop[2] = { type, value };   // 1 int, 4 bool (destructor 0x00A456C0 has nothing to free for those)
	char ok = ((SetPropertyFn)(*(int**)object)[0x2C / 4])(object, 0, str, prop);
	engine_free(str[0]);
	return ok != 0;
}

static std::string adv_map_desc_teams(char* desc, int count) {
	char* players = *(char**)(desc + 0x104);
	int size = (*(char**)(desc + 0x108) - players) / 0xD4;
	std::string text = "CustomTeams " + std::to_string((int)*(desc + 0x100)) + ", Team";
	for (int i = 0; i < size && i < count; ++i) text += " " + std::to_string(*(int*)(players + i * 0xD4 + 0x18));
	return text;
}

struct PakFile { std::string path; ULONGLONG time; };

static void list_paks(const std::string& folder, const char* pattern, std::vector<PakFile>& paks) {
	WIN32_FIND_DATAA data;
	HANDLE find = FindFirstFileA((folder + pattern).c_str(), &data);
	if (find == INVALID_HANDLE_VALUE) return;
	do {
		if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) continue;
		ULONGLONG time = ((ULONGLONG)data.ftLastWriteTime.dwHighDateTime << 32) | data.ftLastWriteTime.dwLowDateTime;
		paks.push_back({ folder + data.cFileName, time });
	} while (FindNextFileA(find, &data));
	FindClose(find);
}

// <Teams> of the template file, from the newest game pak that has it. Game root = folder of bin\H5X.exe's parent.
static void read_teams_from_paks(const std::string& name, const std::string& href,
		std::map<int, std::vector<int>>& entries) {
	std::string file = href.substr(0, href.find('#'));
	if (!file.empty() && file[0] == '/') file.erase(0, 1);
	char exe[MAX_PATH];
	GetModuleFileNameA(NULL, exe, MAX_PATH);
	std::string root = exe;
	root = root.substr(0, root.find_last_of("\\/"));   // bin
	root = root.substr(0, root.find_last_of("\\/") + 1);
	std::vector<PakFile> paks;
	list_paks(root + "data\\", "*.pak", paks);
	list_paks(root + "UserMODs\\", "*.h5u", paks);

	std::string text, source;
	ULONGLONG newest = 0;
	for (const PakFile& pak : paks) {
		libzippp::ZipArchive archive(pak.path);
		if (!archive.open(libzippp::ZipArchive::ReadOnly)) continue;
		libzippp::ZipEntry entry = archive.getEntry(file, true, false);
		if (!entry.isNull() && (source.empty() || pak.time > newest)) {
			text = entry.readAsText();
			source = pak.path;
			newest = pak.time;
		}
		archive.close();
	}
	if (source.empty()) {
		writeLog(WARNING, "RmgTeams: template \"" + name + "\" (" + file + ") not found in " + root
			+ "data\\*.pak / UserMODs\\*.h5u");
		return;
	}
	pugi::xml_document doc;
	if (!doc.load_string(text.c_str())) {
		writeLog(WARNING, "RmgTeams: template \"" + name + "\": cannot parse " + file + " in " + source);
		return;
	}
	pugi::xml_node teams_node = doc.child("RMGTemplate").child("Teams");
	for (pugi::xml_node item : teams_node.children("Item")) {
		std::vector<int> teams;
		for (pugi::xml_node team : item.child("Team").children("Item")) teams.push_back(team.text().as_int());
		int players = item.child("Players").text().as_int();
		entries[players] = teams;
		writeLog(INFO, "RmgTeams: template \"" + name + "\", " + std::to_string(players) + " players: teams "
			+ teams_text(teams) + " (read from " + file + " in " + source + ")");
	}
	if (!teams_node) writeLog(INFO, "RmgTeams: template \"" + name + "\": no Teams in " + file + " in " + source);
}

static void apply_teams(int generator) {
	char* rmap = *(char**)(generator + 0xC);
	char* tmpl = (char*)get_template((int*)(rmap + 0x4C));
	if (!tmpl) return;
	std::string name = template_name(tmpl);
	int count = *(int*)(generator + 0x28);
	auto found = template_teams.find(name);
	if (found == template_teams.end()) {
		char** href = (char**)(tmpl + 0x20);   // DB link +0x0C: href string at link +0x14
		std::string path = (href[0] && href[1] > href[0]) ? std::string(href[0], href[1]) : std::string();
		writeLog(INFO, "RmgTeams: template \"" + name + "\" was loaded before the read hook, reading " + path);
		found = template_teams.insert({ name, {} }).first;
		if (!path.empty()) read_teams_from_paks(name, path, found->second);
	}
	if (found->second.empty()) {
		writeLog(INFO, "RmgTeams: template \"" + name + "\" has no teams, map left without teams");
		return;
	}
	auto entry = found->second.find(count);
	if (entry == found->second.end()) {
		writeLog(INFO, "RmgTeams: template \"" + name + "\" has no teams for " + std::to_string(count)
			+ " players, map left without teams");
		return;
	}
	const std::vector<int>& teams = entry->second;

	std::vector<int> start_zones;
	char* zones = *(char**)(tmpl + 0x5C);
	int zone_count = (*(char**)(tmpl + 0x60) - zones) / 0x74;
	for (int i = 0; i < zone_count; ++i)
		if (*(zones + i * 0x74 + 0x14)) start_zones.push_back(*(int*)(zones + i * 0x74 + 4));
	std::sort(start_zones.begin(), start_zones.end());

	std::vector<int> player_team(count, 0);   // template numbering, 0 = not found
	std::string mapping;
	for (size_t rank = 0; rank < start_zones.size(); ++rank) {
		char* zone = find_game_zone(rmap, start_zones[rank]);
		int player = zone ? *(int*)(zone + 0xF0) : 0;
		if (player < 1 || player > count) continue;
		if (rank >= teams.size()) {
			writeLog(WARNING, "RmgTeams: template \"" + name + "\": player " + std::to_string(player)
				+ " starts in zone " + std::to_string(start_zones[rank]) + " (start zone " + std::to_string(rank + 1)
				+ "), beyond the " + std::to_string(teams.size()) + " teams of the list, map left without teams");
			return;
		}
		player_team[player - 1] = teams[rank];
		mapping += " zone " + std::to_string(start_zones[rank]) + " -> player " + std::to_string(player)
			+ " team " + std::to_string(teams[rank]) + ";";
	}
	for (int p = 0; p < count; ++p) {
		if (player_team[p] < 1) {
			writeLog(WARNING, "RmgTeams: template \"" + name + "\": no start zone or no team for player "
				+ std::to_string(p + 1) + ", map left without teams");
			return;
		}
	}

	int* db = (int*)*editor_db;
	int* object = db ? ((GetEditorObjectFn)(*(int**)db)[0x44 / 4])(db, 0, (int*)(rmap + 0x14)) : nullptr;
	if (!object) {
		writeLog(ERRROR, "RmgTeams: no editor object for the map, map left without teams");
		return;
	}
	object[2]++;
	bool ok = set_property(object, "CustomTeams", 4, 1);
	char path[32];
	for (int i = 0; i < 8; ++i) {
		sprintf_s(path, "players.[%d].Team", i);
		ok = set_property(object, path, 1, i < count ? player_team[i] - 1 : -1) && ok;
	}
	if (--object[2] == 0) release_object(object);

	// Also set the SAdvMapDesc fields FUN_00909ce0 counts the teams from (logged before, to see whether the property
	// writes already reached them).
	char* desc = (char*)dynamic_cast_fn(get_resource((int*)(rmap + 0x14)), 0, rtti_resource, rtti_adv_map_desc, 0);
	std::string desc_before = desc ? adv_map_desc_teams(desc, count) : "no SAdvMapDesc";
	if (desc) {
		*(desc + 0x100) = 1;
		char* players = *(char**)(desc + 0x104);
		int size = (*(char**)(desc + 0x108) - players) / 0xD4;
		for (int i = 0; i < size && i < 8; ++i)
			*(int*)(players + i * 0xD4 + 0x18) = i < count ? player_team[i] - 1 : -1;
	}
	writeLog(ok ? INFO : WARNING, "RmgTeams: template \"" + name + "\", " + std::to_string(count) + " players:"
		+ mapping + (ok ? " applied" : " applied, but a property write failed")
		+ " (map struct after the property writes: " + desc_before + ")");
}

int __fastcall rmg_save_teams(int generator) {
	apply_teams(generator);
	return save_map_teams(generator);
}
