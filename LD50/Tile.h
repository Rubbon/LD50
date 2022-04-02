#pragma once
#include <vector>
#include <string>

enum TileType {
	TT_NONE,
	TT_WATER,
	TT_LAND,
	TT_TREE,

};


struct Tile {
	TileType type = TT_WATER;
	unsigned char owner = 0;
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
	short origin_x;
	short origin_y;
	std::string name;
	std::vector<Pos> myTiles;
};;