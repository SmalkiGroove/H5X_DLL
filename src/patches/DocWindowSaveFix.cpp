#include "pch.h"
#include <cstring>
#include <unordered_set>
#include <vector>

// Fix: single-player saves carried the whole H5X encyclopedia (about 77% of a save).
// A single-player save serializes the interface stack (main menu, Custom Game lobby, adventure map), and with it the
// window tree of every screen. The encyclopedia (/UI/Doc/Root, window DocRootWindow) is a static child of the
// AdventureScreen-Heavy, CustomGameWait, MPWait and HotSeatWait screens, so each saved screen wrote some 160,000
// encyclopedia objects. The encyclopedia holds no game state.
// Save: CWindowScreen::operator& (vtable 0x00E13B10 slot 3) is wrapped. While a screen is written, DocRootWindow is
// unlinked from its children (vector + hash map) and the screen's button groups that belong to the encyclopedia are
// unlinked from the group map, so nothing in the save points into the encyclopedia. Every pointer is restored right
// after the screen is written.
// Load: the screens read from the save are listed. Once the whole save is loaded (FUN_00838510, before the interfaces
// are activated), a screen with no DocRootWindow child gets it re-created from its XDB (Shared children), like
// CWindow::InitByDesc does: FUN_00B8DBC0 creates the tree, IWindow::AddChild (+0x8C) attaches it. Its button groups
// register themselves again through CWindowScreen::GetButtonGroup (FUN_004885F0). Older saves still contain the
// encyclopedia and are loaded as before.
// The list is cleared by FUN_00838940, which each load path (FUN_007EE3A0, FUN_007E8D00, FUN_007F0A90) calls first.

void DocSavePreLoadFork();
void DocSavePostLoadFork();
int __fastcall doc_save_screen_serialize(char* self, void* unused, void* saver);

int DocSaveScreenSerialize_slot = 0x00E13B1C;   // CWindowScreen, CObjectBase vtable (+0x230) slot 3: thunk 0x0048EDC0
int DocSaveScreenSerialize_func = 0x00489AA0;   // CWindowScreen::operator&(IBinSaver&), this = object + 0x230
int DocSavePreLoad_fork = 0x00838940;           // mov ecx,[0FE719Ch] (interface stack teardown, start of each load)
int DocSavePreLoad_return = 0x00838946;
int DocSavePostLoad_fork = 0x00838510;          // mov eax,[0FE719Ch] (end of each load)
int DocSavePostLoad_return = 0x00838515;
int DocSaveDBPtrGet = 0x0088A5A0;               // thiscall CDBPtr<SWindowShared>::Get
int DocSaveChildDescGet = 0x00BE51B0;           // thiscall CDBPtr<SWindow>::Get
int DocSaveCreateWindow = 0x00B8DBC0;           // ecx = SWindow desc, returns CWindow* (with the whole subtree)

void DocWindowSaveFix_init(pugi::xml_document& doc) {
	assembly_patches.push_back({ PATCH_INT, DocSaveScreenSerialize_slot, 4, nullptr, (int)&doc_save_screen_serialize, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DocSavePreLoad_fork, 6, DocSavePreLoadFork, 0, 0, 0 });
	assembly_patches.push_back({ PATCH_HOOK, DocSavePostLoad_fork, 5, DocSavePostLoadFork, 0, 0, 0 });
}

namespace {

const char doc_root_name[] = "DocRootWindow";

// Offsets from the start of a window object. CWindow is the primary base of every window class, and its methods run
// with this = object + 0x10C (IWindow thunks), so these hold for CWindowSimple and CWindowScreen alike.
const int win_vbptr = 0x10;             // vbtable[2] = offset of the IWindow subobject from +0x10
const int win_children_begin = 0xA4;    // vector<CWindow*> (saved as field 9)
const int win_children_end = 0xA8;
const int win_children_map = 0xC4;     // hash_map<CWindow*, bool> (field 10): buckets +4/+8, size +0x10, node {next, key, value}
const int win_shared = 0xF0;           // CDBPtr<SWindowShared>
const int shared_children = 0x48;      // SWindowShared::Children, vector<CDBPtr<SWindow>> (8-byte entries)
const int desc_name = 0x58;            // SWindow::Name {begin, end}
const int screen_this = 0x230;         // CWindowScreen::operator& this
const int screen_groups = 0x1F8;       // hash_map<pair<int, IWindow*>, CObj<group>>: buckets +4/+8, size +0x10, node {next, id, window, group}
const int iwindow_add_child = 0x8C;
const int saver_is_reading = 0x3C;

typedef char* (__thiscall* GetPtrFn)(char*);
typedef char* (__fastcall* CreateWindowFn)(char*);
typedef void(__thiscall* AddChildFn)(char*, char*, char);
typedef char(__thiscall* IsReadingFn)(void*);
typedef int(__thiscall* SerializeFn)(char*, void*);

struct Write { int* addr; int old; };

std::vector<char*> loaded_screens;

int read_int(char* p, int offset) { return *(int*)(p + offset); }

char* iwindow(char* w) {
	int* vbtable = *(int**)(w + win_vbptr);
	return w + win_vbptr + vbtable[2];
}

// The window's DB instance (vtable slot 2), as IWindow +0xE8 (name) reads it
char* instance_desc(char* w) {
	return ((GetPtrFn)(*(void***)w)[2])(w);
}

bool desc_named(char* desc, const char* name) {
	if (desc == nullptr) return false;
	const char* begin = *(char**)(desc + desc_name);
	const char* end = *(char**)(desc + desc_name + 4);
	size_t length = strlen(name);
	if (begin == nullptr || end < begin) return false;
	size_t size = end - begin;
	if (size == length + 1 && begin[length] == 0) size = length;
	return size == length && memcmp(begin, name, length) == 0;
}

bool sane_vector(char* w, int begin_offset, int end_offset, size_t limit) {
	char* begin = (char*)read_int(w, begin_offset);
	char* end = (char*)read_int(w, end_offset);
	return begin <= end && (end - begin) % 4 == 0 && size_t(end - begin) / 4 <= limit && (begin != nullptr || end == nullptr);
}

char* find_doc_root(char* screen) {
	if (!sane_vector(screen, win_children_begin, win_children_end, 4096)) return nullptr;
	char** begin = (char**)read_int(screen, win_children_begin);
	char** end = (char**)read_int(screen, win_children_end);
	for (char** it = begin; it != end; ++it)
		if (*it != nullptr && desc_named(instance_desc(*it), doc_root_name)) return *it;
	return nullptr;
}

void set(std::vector<Write>& undo, int* addr, int value) {
	undo.push_back({ addr, *addr });
	*addr = value;
}

// Unlinks the nodes accepted by `remove` from an engine hash_map (buckets vector at +4/+8, size at +0x10)
template <typename Pred> int unlink_nodes(char* map, std::vector<Write>& undo, Pred remove) {
	int** buckets = *(int***)(map + 4);
	int** buckets_end = *(int***)(map + 8);
	if (buckets == nullptr || buckets_end < buckets || buckets_end - buckets > 1000000) return 0;
	int removed = 0;
	for (int** bucket = buckets; bucket != buckets_end; ++bucket) {
		int* link = (int*)bucket;
		while (*link != 0) {
			int* node = (int*)*link;
			if (remove(node)) {
				set(undo, link, node[0]);
				++removed;
			}
			else {
				link = &node[0];
			}
		}
	}
	if (removed) set(undo, (int*)(map + 0x10), read_int(map, 0x10) - removed);
	return removed;
}

// Unlinks DocRootWindow from the screen, returns false when the screen has none
bool detach_doc(char* screen, std::vector<Write>& undo) {
	char* root = find_doc_root(screen);
	if (root == nullptr) return false;

	// Encyclopedia windows, as objects and as IWindow subobjects (the group map holds IWindow pointers)
	std::unordered_set<char*> doc_windows;
	std::vector<char*> stack{ root };
	while (!stack.empty()) {
		char* w = stack.back();
		stack.pop_back();
		if (!doc_windows.insert(w).second) continue;
		doc_windows.insert(iwindow(w));
		if (!sane_vector(w, win_children_begin, win_children_end, 1 << 16)) continue;
		char** begin = (char**)read_int(w, win_children_begin);
		char** end = (char**)read_int(w, win_children_end);
		for (char** it = begin; it != end; ++it)
			if (*it != nullptr) stack.push_back(*it);
	}

	// Children vector
	char** begin = (char**)read_int(screen, win_children_begin);
	char** end = (char**)read_int(screen, win_children_end);
	char** it = begin;
	while (*it != root) ++it;
	for (; it + 1 != end; ++it) set(undo, (int*)it, (int)it[1]);
	set(undo, (int*)(screen + win_children_end), (int)(end - 1));

	// Children map, groups map
	int children = unlink_nodes(screen + win_children_map, undo, [root](int* node) { return (char*)node[1] == root; });
	int groups = unlink_nodes(screen + screen_groups, undo, [&doc_windows](int* node) { return doc_windows.count((char*)node[2]) != 0; });

	writeLog(INFO, "DocWindowSaveFix: encyclopedia left out of the save (" + std::to_string(doc_windows.size() / 2) +
		" windows, " + std::to_string(children) + " map entry, " + std::to_string(groups) + " button groups)");
	return true;
}

void restore(std::vector<Write>& undo) {
	for (auto it = undo.rbegin(); it != undo.rend(); ++it) *it->addr = it->old;
	undo.clear();
}

// Re-creates DocRootWindow from the screen's XDB when the save had none
void rebuild_doc(char* screen) {
	if (find_doc_root(screen) != nullptr) return;
	char* shared = ((GetPtrFn)DocSaveDBPtrGet)(screen + win_shared);
	if (shared == nullptr) return;
	char* entry = (char*)read_int(shared, shared_children);
	char* entries_end = (char*)read_int(shared, shared_children + 4);
	if (entry == nullptr || entries_end < entry || (entries_end - entry) % 8 != 0) return;
	for (; entry != entries_end; entry += 8) {
		char* desc = ((GetPtrFn)DocSaveChildDescGet)(entry);
		if (!desc_named(desc, doc_root_name)) continue;
		char* window = ((CreateWindowFn)DocSaveCreateWindow)(desc);
		if (window == nullptr) {
			writeLog(ERRROR, "DocWindowSaveFix: the encyclopedia could not be created");
			return;
		}
		char* parent = iwindow(screen);
		((AddChildFn)(*(void***)parent)[iwindow_add_child / 4])(parent, iwindow(window), 0);
		writeLog(INFO, "DocWindowSaveFix: encyclopedia re-created after the load");
		return;
	}
}

} // namespace

// Replaces the thunk 0x0048EDC0 (thiscall, saver on the stack, ret 4)
int __fastcall doc_save_screen_serialize(char* self, void* unused, void* saver) {
	char* object_this = self - *(int*)(self - 4);   // vtordisp, as the thunk does
	char* screen = object_this - screen_this;
	bool reading = ((IsReadingFn)(*(void***)saver)[saver_is_reading / 4])(saver) != 0;
	if (reading) {
		int result = ((SerializeFn)DocSaveScreenSerialize_func)(object_this, saver);
		loaded_screens.push_back(screen);
		return result;
	}
	std::vector<Write> undo;
	detach_doc(screen, undo);
	int result = ((SerializeFn)DocSaveScreenSerialize_func)(object_this, saver);
	restore(undo);
	return result;
}

void __stdcall doc_save_pre_load() {
	loaded_screens.clear();
}

void __stdcall doc_save_post_load() {
	for (char* screen : loaded_screens) rebuild_doc(screen);
	loaded_screens.clear();
}

__declspec(naked) void DocSavePreLoadFork() {
	__asm
	{
		pushad
		call doc_save_pre_load
		popad
		mov ecx, dword ptr ds:[0x00FE719C]
		jmp [DocSavePreLoad_return]
	}
}

__declspec(naked) void DocSavePostLoadFork() {
	__asm
	{
		pushad
		call doc_save_post_load
		popad
		mov eax, dword ptr ds:[0x00FE719C]
		jmp [DocSavePostLoad_return]
	}
}
