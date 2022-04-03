#pragma once
#include <vector>
#include <string>
#include "SDL.h"

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

};


//get tile as bit for comparing buildables
#define TILE_BIT(_t) (1 << ((_t)-1))
#define TILE_BIT_NONE 1 << ((TT_NONE)-1)


struct Tile {
	TileType type = TT_WATER;
	unsigned short ref = 0;	  // these are kinda just generic values to use how you feel
	short timer = 0; // ^
	unsigned char owner = 0;
	unsigned char hp = 0;
};


enum TileInfoFlags {
	TIF_TICKABLE = 0x01,
};

struct TileInfo {
	std::string name = "NAME ME";
	unsigned char baseHp = 0;
	SDL_Rect buildSpr;
	unsigned short buildTime = 0;
	unsigned char flags = 0x00;
};


//extern void OnTileDestroy(int tx = 0, int ty = 0);

extern void TileTick(int x, int y, Tile* _tile);

extern void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile);

extern void DrawLand(int dx, int dy, int tx = 0, int ty = 0);

extern Tile* BuildTileAt(int x, int y, TileType _type);
//ran when the tile has been fully built and has been spawned unto the world
extern void TileOnBuilt(int x, int y, Tile* _tile);

extern bool CheckIfCanBuildTile(int x, int y, TileType _type);




struct Pos {
	short x = 0;
	short y = 0;
};


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
	int friendliness = 8;
	void expandTick();
};





//player HQ
struct Hq {
	unsigned char flags = 0x00;
	short origin_x = -1;
	short origin_y = -1;
};
