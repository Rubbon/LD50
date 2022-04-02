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

