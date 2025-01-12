#pragma once
#include <vector>
#include <string>
#include "SDL.h"
#include "Entity.h"

enum TileType {
	TT_NONE,
	TT_WATER,
	TT_LAND,
	TT_TREE,
	TT_CITYBLOCK_BIG,
	TT_CITYBLOCK_SMALL,

	TT_HQ_TL,
	TT_HQ_TR,
	TT_HQ_BL,
	TT_HQ_BR,

	TT_CITY_BANK,
	TT_CONSTRUCTION_SITE,
	TT_CRATER,

	TT_FACTORY_TL,
	TT_FACTORY_TR,
	TT_FACTORY_BL,
	TT_FACTORY_BR,

	TT_AIRFIELD_TL,
	TT_AIRFIELD_TR,
	TT_AIRFIELD_BL,
	TT_AIRFIELD_BR,

	TT_AA_GUN,
	TT_WALL,
	TT_CITYSTARTER,

	TT_RAIL_STATION_H,
	TT_RAIL_STATION_V,

	TT_RAIL_TRACK,
	//TT_RAIL_TRACK_H,
	//TT_RAIL_TRACK_V,
	//TT_RAIL_TRACK_LU,
	//TT_RAIL_TRACK_RU,
	//TT_RAIL_TRACK_LD,
	//TT_RAIL_TRACK_RD,
	//TT_RAIL_TRACK_CROSS,

	TT_TRAIN_DEPOT,

	TT_BEACON,
	TT_SCANNER,

	TT_ALIEN_HIVE,
	TT_ALIEN_AA_GUN,

};

struct Pos {
	short x = 0;
	short y = 0;
};



//get tile as bit for comparing buildables
#define TILE_BIT(_t) (1 << ((_t)-1))
#define TILE_BIT_NONE 1 << ((TT_NONE)-1)


enum TileFlags {
	TF_ONWATER = 0x01,
};


struct Tile {
	TileType type = TT_WATER;
	unsigned short ref = 0;	  // these are kinda just generic values to use how you feel
	short timer = 0; // ^
	unsigned char owner = 0;
	unsigned char flags = 0x00;
	short hp = 0;
};


enum TileInfoFlags {
	TIF_TICKABLE = 0x01,
	TIF_ALIEN = 0x02,
	TIF_HUMAN = 0x04,
	TIF_WALKABLE = 0x08,
	TIF_RAIL = 0x10,
	TIF_PLAYER = 0x20,
	TIF_CITY = 0x40,
};

//enum TileCategory {
//
//};

//uses timer
enum TileRailStates {
	TRS_H,
	TRS_V,
	TRS_RD,
	TRS_LD,
	TRS_RU,
	TRS_LU,
	TRS_CROSS,
};
// use owner for whether or  not on land


struct TileInfo {
	std::string name = "NAME ME";
	unsigned char baseHp = 0;
	SDL_Rect buildSpr;
	unsigned short buildTime = 0;
	unsigned char flags = 0x00;
	unsigned short buildCost = 0;
	Pos multiTiles = {0,0};
};


//extern void OnTileDestroy(int tx = 0, int ty = 0);

extern void TileTick(int x, int y, Tile* _tile);

extern void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile);

extern void DrawLand(int dx, int dy, int tx = 0, int ty = 0);

extern Tile* BuildTileAt(int x, int y, TileType _type, unsigned char flags = 0x00);

extern void UpdateWallConnections(int x, int y, Tile* _tile);
//ran when the tile has been fully built and has been spawned unto the world
extern void TileOnBuilt(int x, int y, Tile* _tile);

extern bool CheckIfCanBuildTile(int x, int y, TileType _type);

extern void HurtTile(int dmg, int x, int y, Tile* _tile, Entity* _hurtBy = NULL);

extern void OnTileDestroy(int x, int y, Tile* _tile, bool demolished = false, bool multiDestroy = true);

extern void TileDoDefaultDestroy(int x, int y, Tile* _tile, bool demolished = false, bool multiDestroy = true);

enum CityFlags {
	CF_ACTIVE = 0x01,
	CF_FOUND = 0x02,
	CF_HASBANK = 0x04,
};

//cities
struct City {
	unsigned char flags = 0x00;
	unsigned char index = 0;
	short origin_x;
	short origin_y;
	std::string name;
	std::vector<Pos> myTiles;
	int timer = 4;
	int friendliness;
	short resources = 0;
	short maxResources;
	short money = 0;
	short maxMoney=60;
	int popcount = 0;
	int bankX=-1;
	int bankY=-1;
	short warnAboutAttackTimer = 0;
	void expandTick();
};





//player HQ
struct Hq {
	unsigned char flags = 0x00;
	short origin_x = -1;
	short origin_y = -1;
};
