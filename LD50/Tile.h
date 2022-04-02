#pragma once

enum TileType {
	TT_NONE,
	TT_WATER,
	TT_LAND,

};


struct Tile {
	TileType type = TT_WATER;
};



extern void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile);

extern void DrawLand(int dx, int dy, int tx = 0, int ty = 0);