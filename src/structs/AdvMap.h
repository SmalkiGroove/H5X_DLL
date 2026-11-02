#pragma once
#ifndef ADVMAP_H
#define ADVMAP_H

// Adventure map world objects reached from Lua handlers: script owner, world, players, towns / dwellings.
// RE rule: .cursor/rules/h5x-re-lua-engine.mdc.
#include "types/types.h"
#include "types/Functions.h"

typedef void(__thiscall* PostCommand)(int*, int*);
typedef int*(__thiscall* FindObject)(int*, char**);
typedef int*(__thiscall* GetPlayer)(int*, int);
typedef void(__thiscall* SetDwellingCreatures)(int*, int, int);

// NWorld::IPlayer (NWorld::CPlayer subobject +0x1C, vtable 0x00E1A09C), from World_vtable get_player.
enum {
	Player_vtable_addr = 0x00E1A09C
};

struct Player_vtable {
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
	GetInt get_id; // 0x2C (44) - 0x00720E90, returns [this+0x78], player ID 1..8
	int* call_48;
	int* call_52;
	int* call_56;
	int* call_60;
	int* call_64;
	int* call_68;
	int* call_72;
	int* call_76;
	int* call_80;
	int* call_84;
	int* call_88;
	int* call_92;
	int* call_96;
	int* call_100;
	int* call_104;
	GetInt is_active; // 0x6C (108) - 0x00C61A70, returns [this+0x7C], 0 when the player doesn't take part in the game
	int* call_112;
	int* call_116;
	int* call_120;
	int* call_124;
	int* call_128;
	int* call_132;
	int* call_136;
	int* call_140;
	int* call_144;
	int* call_148;
	int* call_152;
	int* call_156;
	int* call_160;
	int* call_164;
	int* call_168;
	int* call_172;
	int* call_176;
	int* call_180;
	int* call_184;
	int* call_188;
	int* call_192;
	int* call_196;
	int* call_200;
	int* call_204;
	int* call_208;
	int* call_212;
	int* get_resources; // 0xD8 (216) - 0x00BA03D0, int[7] resources (wood ... gold), read by CAddPlayerMoneyCmd
	int* set_resources; // 0xDC (220) - 0x004FC3E0, written back by CAddPlayerMoneyCmd
	int* call_224;
	int* call_228;
	int* call_232;
	int* call_236;
	int* call_240;
	int* call_244;
	int* call_248;
	int* call_252;
	int* call_256;
	int* call_260;
	int* call_264;
	int* call_268;
	int* call_272;
	int* call_276;
	int* call_280;
	int* call_284;
	int* call_288;
	int* call_292;
	int* call_296;
	int* call_300;
	int* call_304;
	int* call_308;
	int* call_312;
	int* call_316;
	int* call_320;
	int* call_324;
	int* call_328;
	int* call_332;
	int* call_336;
	int* call_340;
	int* call_344;
	int* call_348;
	int* call_352;
	int* call_356;
	int* call_360;
	int* call_364;
	int* call_368;
	int* call_372;
	int* call_376;
	int* call_380;
	int* call_384;
	GetFlag is_human_player; // 0x184 (388) - 0x00500D60, [this+0x74] == 0
	GetInt get_host_type; // 0x188 (392) - 0x00B7F260, returns [this+0x74], 0 = HUMAN
};

struct IPlayer {
	Player_vtable* instance;

	int id() {
		return instance->get_id((int*)this);
	}
	bool is_active() {
		return instance->is_active((int*)this) != 0;
	}
	bool is_human() {
		return instance->get_host_type((int*)this) == 0;
	}
};

// NWorld::CWorld (vtable 0x00E6260C, primary), from AdventureMap_vtable get_world.
enum {
	World_vtable_addr = 0x00E6260C
};

struct World_vtable {
	int* call_0;
	int* call_4;
	int* call_8;
	int* call_12;
	int* call_16;
	GetPtr get_game_params; // 0x14 (20) - 0x0098C470, returns &[this+0x88]: +0x00 multiplayer flag, +0x01 hotseat flag
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
	int* call_68;
	int* call_72;
	int* call_76;
	int* call_80;
	int* call_84;
	int* call_88;
	int* call_92;
	int* call_96;
	int* call_100;
	GetPlayer get_player; // 0x68 (104) - 0x0098D2C0, thiscall(player ID 1..8) -> IPlayer, nullptr when absent
};

struct IWorld {
	World_vtable* instance;

	char* game_params() {
		return (char*)instance->get_game_params((int*)this);
	}
	IPlayer* player(int id) {
		return (IPlayer*)instance->get_player((int*)this, id);
	}
};

// Town / dwelling recruits, as used by the Lua handlers SetObjectDwellingCreatures (0x005CD270),
// GetObjectDwellingCreatures (0x005CD530) and NWorld::CSetDwellingCreaturesCmd execute (0x00427C00).
// Concrete vtable not identified (an NWorld::CAdvMapTown / CAdvMapDwelling subobject).
struct Dwelling_vtable {
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
	int* call_68;
	int* call_72;
	int* call_76;
	int* call_80;
	int* call_84;
	int* call_88;
	int* call_92;
	int* call_96;
	int* call_100;
	int* call_104;
	int* call_108;
	int* call_112;
	int* call_116;
	int* call_120;
	int* call_124;
	int* call_128;
	int* call_132;
	int* call_136;
	int* call_140;
	int* call_144;
	int* call_148;
	int* call_152;
	int* call_156;
	int* call_160;
	int* call_164;
	int* call_168;
	int* call_172;
	int* call_176;
	int* call_180;
	int* call_184;
	int* call_188;
	int* call_192;
	int* call_196;
	int* call_200;
	int* call_204;
	int* call_208;
	int* call_212;
	int* call_216;
	int* call_220;
	int* call_224;
	int* call_228;
	int* call_232;
	int* call_236;
	int* call_240;
	int* call_244;
	int* call_248;
	SetDwellingCreatures set_creatures; // 0xFC (252) - thiscall(creature ID, count): sets the recruits of the creature
	CheckValue get_creatures; // 0x100 (256) - thiscall(creature ID) -> recruits of the creature
};

struct IDwelling {
	Dwelling_vtable* instance;

	int creatures(int creature) {
		return instance->get_creatures((int*)this, creature);
	}
	void set_creatures(int creature, int count) {
		instance->set_creatures((int*)this, creature, count);
	}
};

// Adventure map object, from AdventureMap_vtable find_object. Concrete vtable not identified.
struct MapObject_vtable {
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
	GetPtr get_dwelling; // 0x3C (60) - IDwelling of a town / dwelling (check it with engine_object_alive)
};

struct IMapObject {
	MapObject_vtable* instance;

	IDwelling* dwelling() {
		return (IDwelling*)instance->get_dwelling((int*)this);
	}
};

// NAdventureMapScript::IAdventureMap (RTTI type descriptor 0x00F34FCC): owner of the "advmap" Lua engine.
// Implementations: NUI::CAdventureController (vtable 0x00E29A2C, single player) and NWorld::CWorldScriptSystem
// (vtable 0x00E61D14, multiplayer / hotseat).
enum {
	AdventureController_vtable_addr = 0x00E29A2C,
	WorldScriptSystem_vtable_addr = 0x00E61D14
};

struct AdventureMap_vtable {
	GetPtr get_world; // 0x00 (0) - controller 0x00819280 (returns [this+0x18]); script system 0x00724930 — IWorld
	PostCommand post_command; // 0x04 (4) - controller 0x006035D0 (transceiver at +0x1C); script system 0x0098D050 — thiscall(world command): executed with the next world segment in single player, at once by the world script system
	int* call_8;
	CheckValue random; // 0x0C (12) - controller 0x006035E0; script system 0x0098D090 — thiscall(n) -> [0, n)
	int* call_16;
	int* call_20;
	int* call_24;
	FindObject find_object; // 0x1C (28) - controller 0x00603400 (world vtable +0x140, then +0x20); script system 0x0098D0E0 — thiscall(engine string* name) -> map object
};

struct IAdventureMap {
	AdventureMap_vtable* instance;

	IWorld* world() {
		return (IWorld*)instance->get_world((int*)this);
	}
	void post_command(int* command) {
		instance->post_command((int*)this, command);
	}
	int random(int range) {
		return instance->random((int*)this, range);
	}
	IMapObject* find_object(const char* name) {
		char* str[3] = { nullptr, nullptr, nullptr };
		make_engine_string(str, 0, name, name + strlen(name));
		IMapObject* object = (IMapObject*)instance->find_object((int*)this, str);
		engine_free(str[0]);
		return object;
	}
};

#endif
