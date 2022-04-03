#include "Tile.h"
#include "Graphics.h"
#include "Game.h"
#include "TileBaseInfo.h"





void TileTick(int x, int y, Tile* _tile) {
	switch (_tile->type) {
		default: return;
		case TT_CONSTRUCTION_SITE:
			if (_tile->timer < GET_TILE_INFO(_tile->ref).buildTime) {
				_tile->timer++;
			} else {
				_tile->timer = 0;
				_tile->type = (TileType) _tile->ref;
				_tile->ref = 0;
				_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
				TileOnBuilt(x, y, _tile);
			}
		break;
	}
}




void TileDraw(int dx, int dy, int tx, int ty, Tile* _tile) {
	switch (_tile->type) {
		case TT_WATER:
			//Graphics::DrawSpr(TEX_CHARS, { dx + (GAME_TICK/40) % 8, dy, 8, 8 }, { 8, 0, 8, 8 });
			Graphics::DrawSpr(TEX_CHARS, { dx + 3 + (int)(sin(ty + GAME_TICK*0.01f)*6), dy, 8, 8 }, { 8, 0, 8, 8 });
		break;
		case TT_LAND:
			DrawLand(dx, dy, tx, ty);
		break;
		case TT_TREE:
			DrawLand(dx, dy, tx, ty);
			//Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + ((int)(tx - sin(ty)) % 2) * 8, 40, 8, 8});
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + (ty%2) * 8, 40, 8, 8});
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

		case TT_HQ_TL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 64, 8, 8 });
		break;
		case TT_HQ_TR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 16 + ((GAME_TICK/12)%4)*8, 64, 8, 8 });
		break;
		case TT_HQ_BL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 72, 8, 8 });
		break;
		case TT_HQ_BR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 16, 72, 8, 8 });
		break;
		
		case TT_CONSTRUCTION_SITE: {
			DrawLand(dx, dy, tx, ty);

			int _tick = (float)_tile->timer / GET_TILE_INFO(_tile->ref).buildTime * 4;
			if (_tick > 3) _tick = 3;

			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + _tick * 8, 80, 8, 8 });
		break; }

		case TT_CRATER:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 32, 8, 8 });
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

bool CheckIfCanBuildTile(int x, int y, TileType _type) {
	switch (_type) {
		default: 
			if (LEVEL.GetTile(x, y)->type == TT_LAND || LEVEL.GetTile(x, y)->type == TT_CRATER) return true;
		return false;
		case TT_HQ_TL:
			if (LEVEL.GetTile(x, y)->type == TT_LAND && LEVEL.GetTile(x+1, y)->type == TT_LAND && LEVEL.GetTile(x, y + 1)->type == TT_LAND && LEVEL.GetTile(x + 1, y + 1)->type == TT_LAND) return true;
		return false;


	}
	return false;
}



Tile* BuildTileAt(int x, int y, TileType _type) {

	Tile* _tile;

	switch (_type) {
		default: 
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			LEVEL.vTilesToTick.push_back({(short)x, (short)y});
		break;
		case TT_HQ_TL:
			//tl
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			//tr
			_tile = LEVEL.GetTile(x + 1, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_TR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			//bl
			_tile = LEVEL.GetTile(x, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_BL;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			//br
			_tile = LEVEL.GetTile(x + 1, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_BR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;

			//dont look atr this its horrible \/
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)y });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)y });
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)(y + 1) });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)(y + 1) });

			GAME.state = GS_PLAY;

		break;
	}

	return _tile;

}


void TileOnBuilt(int x, int y, Tile* _tile) {
	switch (_tile->type) {
		case TT_HQ_TL:
			//set hq to here
			LEVEL.playerHq.origin_x = x;
			LEVEL.playerHq.origin_y = y;
			LEVEL.playerHq.flags |= CF_ACTIVE;
		break;
	}
}


void HurtTile(int dmg, int x, int y, Tile* _tile) {
	_tile->hp -= dmg;

	if (_tile->hp <= 0) OnTileDestroy(x, y, _tile);
}



void OnTileDestroy(int x, int y, Tile* _tile) {

	switch (_tile->type) {
		default:
			_tile->type = TT_CRATER;
			_tile->owner = 0;
		break;
	}
}








void City::expandTick() {
	//std::cout << timer << std::endl;
	
	int _popCount = 0;
	for (int i = 0; i < myTiles.size(); i++) {
		Tile* _curTile = GAME.currentLevel.GetTile(myTiles[i].x, myTiles[i].y);
		if (_curTile->owner != index) {
			//check if city has been gooped by aliens
			myTiles.erase(myTiles.begin() + i);
			timer = 10;
		}
		if (_curTile->type == TT_CITYBLOCK_SMALL) _popCount += (4000 + (int)rand() % 999);
		else if (_curTile->type == TT_CITYBLOCK_BIG) _popCount += (10000 + (int)rand() % 9999);
		else if (_curTile->type == TT_CITY_BANK) {
			_popCount += (10000 + (int)rand() % 9999);
		}
		friendliness--;
		
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
				_t = BuildTileAt(_placeX,_placeY, _cityBlockType);
				_t->owner = index;
				break;
			}
			else {
				_placeX += (rand() % 3) - 1;
				_placeY += (rand() % 3) - 1;
				if (_placeX == 0 && _placeY == 0) {
					_placeX = -1;
					_placeY = 1;
				}
				//chance to upgrade existing city tile
				_chance = (int)rand() % 10;
				if (_chance > 1) {
					if (_t->type == TT_CITYBLOCK_SMALL) { 
						_t = BuildTileAt(_placeX, _placeY, TT_CITYBLOCK_BIG);
						_t->owner = index;
						myTiles.push_back({ (short)_placeX, (short)_placeY });
						break;
					}
					else if (_t->type == TT_CITYBLOCK_BIG && _popCount>160000) {
						if (!(flags & CF_HASBANK)) {
							flags|= CF_HASBANK;
							_t = BuildTileAt(_placeX, _placeY, TT_CITY_BANK);
							_t->owner = index;
							myTiles.push_back({ (short)_placeX, (short)_placeY });
							break;
						}
					}
				}
			}
		}
		timer = 4;
	}
}