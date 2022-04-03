#include "Tile.h"
#include "Graphics.h"
#include "Game.h"

void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile) {
	switch (_tile->type) {
		case TT_WATER:
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 0, 8, 8 });
		break;
		case TT_LAND:
			DrawLand(dx, dy, tx, ty);
		break;
		case TT_TREE:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + ((int)(tx - sin(ty)) % 2) * 8, 40, 8, 8});
		break;
		case TT_CITYBLOCK_BIG:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + ((int)(tx + sin(ty-tx)) % 3) * 8, 48, 8, 8 });
		break;
		case TT_CITYBLOCK_SMALL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + ((int)(tx + sin(ty-tx)) % 3) * 8, 56, 8, 8 });
		break;
		case TT_CITY_BANK:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 56, 48, 8, 8 });
		break;

	}
}


void DrawLand(int dx, int dy, int tx, int ty) {

	int _sprX = 16;
	int _sprY = 16;

	//no left tile
	if (GAME.currentLevel.GetTile(tx - 1, ty)->type == TT_WATER) {
		_sprX -= 8;
	} else
	//no right tile
	if (GAME.currentLevel.GetTile(tx + 1, ty)->type == TT_WATER) {
		_sprX += 8;
	}

	//no top
	if (GAME.currentLevel.GetTile(tx, ty - 1)->type == TT_WATER) _sprY -= 8;
	//no bot
	else if (GAME.currentLevel.GetTile(tx, ty + 1)->type == TT_WATER) _sprY += 8;


	if ((int)(tx + ty * (1+sin(tx-ty))) % 8 <= 0) _sprX += 24;

	Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { _sprX, _sprY, 8, 8 });


}

void City::expandTick() {
	//check if city has been gooped by aliens
	for (int i = 0; i < myTiles.size(); i++) {
		if (GAME.currentLevel.GetTile(myTiles[i].x, myTiles[i].y)->owner != index) myTiles.erase(myTiles.begin() + i);
		timer = 20;
	}
	
	if (timer > 0) timer--;
	else timer = 12;
}