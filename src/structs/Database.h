#pragma once
#ifndef DATABASE_H
#define DATABASE_H

// XDB objects: XML chunk saver (DB serializers) and the editor database / object property interface used by the
// random map generator. RE rule: .cursor/rules/h5x-re-rmg.mdc.
#include "types/types.h"

typedef char(__thiscall* SaverStartChunk)(int*, const char*, int);
typedef void(__thiscall* SaverFinishChunk)(int*);
typedef int*(__thiscall* GetEditorObject)(int*, int*);
typedef char(__thiscall* SetEditorProperty)(int*, char**, int*);

// CXMLChunkSaver (vtable 0x00E6668C): saver passed to the DB serializers (operator&) when reading / writing .xdb.
enum {
	XmlChunkSaver_vtable_addr = 0x00E6668C
};

struct XmlChunkSaver_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	GetInt get_child_count; // 0x1C (28) - 0x009E21C0, children of the current chunk
	int* call_32;
	int* call_36;
	int* call_40;
	int* call_44;
	int* call_48;
	int* call_52;
	int* call_56;
	int* read_string; // 0x3C (60) - 0x009E3250, char string {begin, end, capacity}
	int* call_64;
	int* call_68;
	GetFlag is_reading; // 0x48 (72) - 0x00C61AD0, returns [this+0x78]
	int* call_76;
	SaverStartChunk start_chunk; // 0x50 (80) - 0x009E25D0, thiscall(name, index) -> char, true when the chunk exists
	SaverFinishChunk finish_chunk; // 0x54 (84) - 0x009E2200
};

struct IXmlChunkSaver {
	XmlChunkSaver_vtable* instance;

	int child_count() {
		return instance->get_child_count((int*)this);
	}
	bool is_reading() {
		return instance->is_reading((int*)this);
	}
	bool start_chunk(const char* name, int index) {
		return instance->start_chunk((int*)this, name, index) != 0;
	}
	void finish_chunk() {
		instance->finish_chunk((int*)this);
	}
};

// Editor object of a DB resource (EditorDatabase_vtable get_object). Concrete vtable not identified.
struct EditorObject_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	int* call_28;
	int* call_32;
	int* call_36;
	int* call_40;
	SetEditorProperty set_property; // 0x2C (44) - thiscall(engine string* path, {type, value}* value) -> char; type 1 int, 2 float, 4 bool
	int* call_48;
	int* insert_item; // 0x34 (52) - thiscall(engine string* path, index, 1, 0): inserts an array item
};

struct IEditorObject {
	EditorObject_vtable* instance;

	bool set_property(char** path, int* value) {
		return instance->set_property((int*)this, path, value) != 0;
	}
};

// NDb::CEditorDatabase (vtable 0x00E8F5AC), [0x0103FFE8] (returned by 0x00BA06C0).
enum {
	EditorDatabase_vtable_addr = 0x00E8F5AC
};

struct EditorDatabase_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	int* call_20;
	int* call_24;
	int* call_28;
	int* call_32;
	int* call_36;
	int* call_40;
	int* call_44;
	int* call_48;
	int* call_52;
	int* call_56;
	int* call_60;
	int* call_64;
	GetEditorObject get_object; // 0x44 (68) - 0x00C6D850, thiscall(DB path string*) -> editor object (refcount at +8)
};

struct IEditorDatabase {
	EditorDatabase_vtable* instance;

	IEditorObject* object(int* path) {
		return (IEditorObject*)instance->get_object((int*)this, path);
	}
};

#endif
