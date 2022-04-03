#pragma once
#include <vector>
#include <string>

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

};


struct Tile {
	TileType type = TT_WATER;
	unsigned char owner = 0;
	unsigned char hp = 0;
};



extern void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile);

extern void DrawLand(int dx, int dy, int tx = 0, int ty = 0);







struct Pos {
	short x = 0;
	short y = 0;
};


enum CityFlags {
	CF_ACTIVE = 0x01,
};

//cities
struct City {
	unsigned char flags = 0x00;
	unsigned char index = 0;
	short origin_x;
	short origin_y;
	std::string name;
	std::vector<Pos> myTiles;
	int timer = 1;
	int friendliness = 8;
	void expandTick();
};

/*
//player HQ
struct Hq {
	unsigned char flags = 0x00;
	short origin_x;
	short origin_y;
};
*/