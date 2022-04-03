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
	int _popCount = 0;
	bool _hasBank = false;
	for (int i = 0; i < myTiles.size(); i++) {
		Tile* _curTile = GAME.currentLevel.GetTile(myTiles[i].x, myTiles[i].y);
		if (_curTile->owner != index) {
			myTiles.erase(myTiles.begin() + i);
			if (_curTile->type == TT_CITYBLOCK_SMALL) _popCount += (4000 + (int)rand() % 999);
			else if (_curTile->type==TT_CITYBLOCK_BIG) _popCount += (10000 + (int)rand() % 9999);
			else if (_curTile->type == TT_CITY_BANK) {
				_popCount += (10000 + (int)rand() % 9999);
				_hasBank = true;
			}
		}
		friendliness--;
		timer = 20;
	}
	
	if (timer > 0) timer--;
	else {
		//time to grow
		Tile* _t;
		int _placeX = origin_x;
		int _placeY = origin_y;
		int _chance;
		for (int i = 0; i < myTiles.size() * 2; i++) {
			_t = LEVEL.GetTile(_placeX, _placeY);
			if (_t->type == TT_LAND) {
				TileType _cityBlockType = TT_CITYBLOCK_BIG;
				if (i > myTiles.size()) _cityBlockType = TT_CITYBLOCK_SMALL;

				_t->type = _cityBlockType;
				_t->owner = index;
			}
			else {
				_placeX += (rand() % 3) - 1;
				_placeY += (rand() % 3) - 1;
				if (_placeX == 0 && _placeY == 0) {
					_placeX = -1;
					_placeY = 1;
				}
				//chance to existing city tile
				_chance = (int)rand() % 10;
				if (_chance > 7) {
					if (_t->type == TT_CITYBLOCK_SMALL) { 
						_t->type = TT_CITYBLOCK_BIG; 
						_t->owner = index;
					}
					else if (_t->type == TT_CITYBLOCK_BIG && !_hasBank) {
						_t->type = TT_CITY_BANK;
						_t->owner = index;
						//myTiles.push_back(_t);
					}
				}
			}
		}
		timer = 1;
	}
}