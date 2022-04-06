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

		case TT_AA_GUN: {
			if ((x + y + GAME_TICK) % 32 == 0) {

				Entity* _ent = GetEntityInDistFlags((x*8) + 4, (y*8) + 4, 64, EFL_ALIEN);
				
				if (_ent != NULL) {
					//make bullet
					Entity* _bul = LEVEL.AddEntity((x * 8) + 2, (y * 8) + 2, ENT_JETBULLET, false);
					if (PosIsOnScreen(x * 8, y * 8)) Sound::PlayTempSoundAt(SND_BULLET, _ent->x, _ent->y);
					float _angle = atan2(_ent->y - _bul->y, _ent->x - _bul->x);
					_bul->mx = 4 * cos(_angle);
					_bul->my = 4 * sin(_angle);
					_bul->z = _ent->z;
					arrEntityFuncs[_bul->entityIndex].Init(_bul);
				}

			}
		break; }

		case TT_TRAIN_DEPOT: {
			if (_tile->ref == 0 || LEVEL.arrEntities[_tile->ref].flags & EFL_DELETED) {

				int _xMake = -1;
				int _yMake = -1;

				//make new train
	
				//make one to right
				if (GET_TILE_INFO(LEVEL.GetTile(x + 1, y)->type).flags & TIF_RAIL) {
					_xMake = x + 1;
					_yMake = y;
				} else
				if (GET_TILE_INFO(LEVEL.GetTile(x, y + 1)->type).flags & TIF_RAIL) {
					_xMake = x;
					_yMake = y + 1;
				} else
				if (GET_TILE_INFO(LEVEL.GetTile(x - 1, y)->type).flags & TIF_RAIL) {
					_xMake = x - 1;
					_yMake = y;
				} else
				if (GET_TILE_INFO(LEVEL.GetTile(x, y - 1)->type).flags & TIF_RAIL) {
					_xMake = x;
					_yMake = y - 1;
				}


				if (_xMake == -1) break;

				Entity* _ent = LEVEL.AddEntity(_xMake * 8, _yMake * 8, ENT_TRAIN);
				_tile->ref = _ent->id;
				_ent->fx = x;
				_ent->fy = y;
			}
		break; }

		case TT_AIRFIELD_TL: case TT_AIRFIELD_BR: {
			if (_tile->ref == 0 || LEVEL.arrEntities[_tile->ref].flags & EFL_DELETED) {
				if (_tile->timer == 0) {
					Entity* _ent = LEVEL.AddEntity(x * 8, y * 8, ENT_MILJET);
					_tile->ref = _ent->id;
					_ent->fx = x + (rand() % 6) - 3;
					_ent->fy = y + (rand() % 6) - 3;
					_ent->target_x = x * 8;
					_ent->target_y = y * 8;
					_tile->timer = 64;
				} else {
					_tile->timer--;
				}
			}
		break; }

		case TT_SCANNER: {
			if (_tile->timer <= 0) {

				Entity* _ent = GetEntityInDistFlags((x * 8) + 4, (y * 8) + 4, 80, EFL_ALIEN);

				if (_ent != NULL) {
					GAME.AddNews("Aliens spotted around " + std::to_string(x) + ", " + std::to_string(y) + "!");
				}

				//see hive
				if (_tile->ref != 128) {
					if (LEVEL.CheckForTileTypeInRange(x, y, 6, TIF_ALIEN)) GAME.AddNews("Alien hive spotted growing around " + std::to_string(x) + ", " + std::to_string(y) + "!");
					_tile->ref = 128;
				}

				_tile->timer = 256;
			} else {
				_tile->timer--;
			}
		break; }

		case TT_ALIEN_AA_GUN: {
			if ((x + y + GAME_TICK) % 40 == 0) {
				
				Entity* _ent = GetEntityInDistFlags((x * 8) + 4, (y * 8) + 4, 64, EFL_HUMAN);

				if (_ent != NULL) {
					//make bullet
					Entity* _bul = LEVEL.AddEntity((x * 8) + 2, (y * 8) + 6, ENT_ALIENBULLET, false);
					if (PosIsOnScreen(x * 8, y * 8)) Sound::PlayTempSoundAt(SND_BULLET, _ent->x, _ent->y);
					float _angle = atan2(_ent->y - _bul->y, _ent->x - _bul->x);
					_bul->mx = 3 * cos(_angle);
					_bul->my = 3 * sin(_angle);
					_bul->wait = 32;
					_bul->z = _ent->z;
					arrEntityFuncs[_bul->entityIndex].Init(_bul);
				}


				//consider spawning a walker
				if (_tile->timer <= 0) {

					if (rand() % 3 == 0) {
						LEVEL.AddEntity(x * 8, y * 8, ENT_WALKER);
					}

					_tile->timer = 128 + rand() % 64;
				} else {
					_tile->timer --;
				}

			}
		break; }



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
			if (!(_tile->flags & TF_ONWATER)) DrawLand(dx, dy, tx, ty);

			int _tick = (float)_tile->timer / GET_TILE_INFO(_tile->ref).buildTime * 4;
			if (_tick > 3) _tick = 3;

			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + _tick * 8, 80, 8, 8 });
		break; }

		case TT_CRATER:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 32, 8, 8 });
		break;


		case TT_AIRFIELD_TL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 0, 128, 8, 8 });
		break;
		case TT_AIRFIELD_TR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 128, 8, 8 });
		break;
		case TT_AIRFIELD_BL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 0, 136, 8, 8 });
		break;
		case TT_AIRFIELD_BR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8, 136, 8, 8 });
		break;

		case TT_FACTORY_TL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 48, 64, 8, 8 });
			break;
		case TT_FACTORY_TR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 56, 64, 8, 8 });
			break;
		case TT_FACTORY_BL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 48, 72, 8, 8 });
			break;
		case TT_FACTORY_BR:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 56, 72, 8, 8 });
			break;


		case TT_RAIL_STATION_H:
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 88, 88, 8, 8 });
		break;

		case TT_RAIL_TRACK:
			if (!(_tile->flags & TF_ONWATER)) DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 104 + _tile->timer*8, 88, 8, 8 });
		break;

		case TT_AA_GUN:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, {((GAME_TICK / 96) % 2) * 8,96, 8, 8 });
		break;

		case TT_WALL:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 64+_tile->timer*8, 96, 8, 8 });
			break;
		case TT_CITYSTARTER:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 56, 56, 8, 8 });
		break;

		case TT_TRAIN_DEPOT:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 160, 88, 8, 8 });
		break;

		case TT_SCANNER:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 56 + ((GAME_TICK / 96) % 2) * 8, 104, 8, 8 });
		break;

		case TT_ALIEN_HIVE:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 8 + ((int)(tx + sin(ty - tx)) % 4) * 8, 144, 8, 8 });
		break;

		case TT_ALIEN_AA_GUN:
			DrawLand(dx, dy, tx, ty);
			Graphics::DrawSpr(TEX_CHARS, { dx, dy, 8, 8 }, { 24 + ((GAME_TICK / 96) % 2) * 8, 152, 8, 8 });
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

	//see if we have enough money
	if (GAME.playerCash < GET_TILE_INFO(_type).buildCost) return false;

	switch (_type) {
		default: 
			if (LEVEL.GetTile(x, y)->type == TT_LAND || LEVEL.GetTile(x, y)->type == TT_CRATER) return true;
		return false;
		case TT_HQ_TL: case TT_AIRFIELD_TL: case TT_FACTORY_TL:
			if (LEVEL.GetTile(x, y)->type == TT_LAND && LEVEL.GetTile(x+1, y)->type == TT_LAND && LEVEL.GetTile(x, y + 1)->type == TT_LAND && LEVEL.GetTile(x + 1, y + 1)->type == TT_LAND) return true;
		return false;
		case TT_RAIL_TRACK:
			if (LEVEL.GetTile(x, y)->type == TT_LAND || LEVEL.GetTile(x, y)->type == TT_CRATER || LEVEL.GetTile(x, y)->type == TT_WATER) return true;
		break;

	}
	return false;
}



Tile* BuildTileAt(int x, int y, TileType _type, unsigned char flags) {

	Tile* _tile;

	switch (_type) {
		default: 
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			LEVEL.vTilesToTick.push_back({(short)x, (short)y});
		break;
		case TT_HQ_TL:
			//tl
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//tr
			_tile = LEVEL.GetTile(x + 1, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_TR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//bl
			_tile = LEVEL.GetTile(x, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_BL;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//br
			_tile = LEVEL.GetTile(x + 1, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_HQ_BR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;

			//dont look atr this its horrible \/
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)y });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)y });
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)(y + 1) });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)(y + 1) });
		
			//GAME.state = GS_PLAY;
			GAME.state = GS_BUILD;

			LEVEL.playerHq.origin_x = x;
			LEVEL.playerHq.origin_y = y;
			LEVEL.playerHq.flags |= CF_ACTIVE;

		break;
		case TT_FACTORY_TL:
			//tl
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//tr
			_tile = LEVEL.GetTile(x + 1, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_FACTORY_TR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//bl
			_tile = LEVEL.GetTile(x, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_FACTORY_BL;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//br
			_tile = LEVEL.GetTile(x + 1, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_FACTORY_BR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;

			//dont look atr this its horrible \/
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)y });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)y });
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)(y + 1) });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)(y + 1) });

		break;
		case TT_AIRFIELD_TL:
			//tl
			_tile = LEVEL.GetTile(x, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = _type;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//tr
			_tile = LEVEL.GetTile(x + 1, y);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_AIRFIELD_TR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//bl
			_tile = LEVEL.GetTile(x, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_AIRFIELD_BL;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;
			//br
			_tile = LEVEL.GetTile(x + 1, y + 1);
			_tile->type = TT_CONSTRUCTION_SITE;
			_tile->ref = TT_AIRFIELD_BR;
			_tile->hp = GET_TILE_INFO(_tile->type).baseHp;
			_tile->owner = 0;
			_tile->flags = flags;

			//dont look atr this its horrible \/
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)y });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)y });
			LEVEL.vTilesToTick.push_back({ (short)x,		(short)(y + 1) });
			LEVEL.vTilesToTick.push_back({ (short)(x + 1),	(short)(y + 1) });

		break;




	}

	return _tile;

}

void UpdateWallConnections(int x, int y, Tile* _tile) {
	Tile* _tcheckL;
	Tile* _tcheckR;
	Tile* _tcheckU;
	Tile* _tcheckD;
	bool _connected_above = false;
	bool _connected_below = false;
	bool _connected_left = false;
	bool _connected_right = false;
	_tcheckL = LEVEL.GetTile(x - 1, y);
	_tcheckR = LEVEL.GetTile(x + 1, y);
	_tcheckU = LEVEL.GetTile(x, y - 1);
	_tcheckD = LEVEL.GetTile(x, y + 1);
	if (_tcheckL->type == TT_WALL) _connected_left = true;
	if (_tcheckR->type == TT_WALL) _connected_right = true;
	if (_tcheckU->type == TT_WALL) _connected_above = true;
	if (_tcheckD->type == TT_WALL) _connected_below = true;
	if (_connected_left && _connected_right) {
		if (_connected_above || _connected_below) _tile->timer = 0;
		else _tile->timer = 1;
	}
	else if (_connected_above && _connected_below) {
		if (_connected_left || _connected_right) _tile->timer = 0;
		else _tile->timer = 2;
	}
	//else _tile->timer = 0;
}



void TileOnBuilt(int x, int y, Tile* _tile) {
	switch (_tile->type) {
		case TT_HQ_TL:
			//set hq to here
			LEVEL.playerHq.origin_x = x;
			LEVEL.playerHq.origin_y = y;
			LEVEL.playerHq.flags |= CF_ACTIVE;
		break;

		case TT_RAIL_STATION_H: {
			//check  what type
			//Tile* _t = LEVEL.GetTile(x - 1, y);


		break; }
		
		case TT_WALL: {
			Tile* _tcheckL;
			Tile* _tcheckR;
			Tile* _tcheckU;
			Tile* _tcheckD;
			bool _connected_above = false;
			bool _connected_below = false;
			bool _connected_left = false;
			bool _connected_right = false;
			_tcheckL = LEVEL.GetTile(x-1, y);
			_tcheckR = LEVEL.GetTile(x +1, y);
			_tcheckU = LEVEL.GetTile(x,y-1);
			_tcheckD = LEVEL.GetTile(x,y+1);
			if (_tcheckL->type == TT_WALL) _connected_left = true;
			if (_tcheckR->type == TT_WALL) _connected_right = true;
			if (_tcheckU->type == TT_WALL) _connected_above = true;
			if (_tcheckD->type == TT_WALL) _connected_below = true;

			if (_connected_left && _connected_right) {
				if (_connected_above || _connected_below) _tile->timer = 0;
				else _tile->timer = 1;
			}
			else if (_connected_above && _connected_below) {
				if (_connected_left || _connected_right) _tile->timer = 0;
				else _tile->timer = 2;
			}
			UpdateWallConnections(x - 1, y, _tcheckL);
			UpdateWallConnections(x + 1, y, _tcheckR);
			UpdateWallConnections(x, y - 1, _tcheckU);
			UpdateWallConnections(x, y + 1, _tcheckD);
			break;
		}

		case TT_RAIL_TRACK: {

			//DOGSHIT CODE WARNING

			Tile* _tcheck0;
			Tile* _tcheck1;
			Tile* _tcheck2;

			bool _connected_above = false;
			bool _connected_below = false;

			//check for above tile to connect to
			_tcheck0 = LEVEL.GetTile(x, y - 1);
			if (_tcheck0->type == TT_RAIL_TRACK) {

				//easy connect, straight shot
				if (_tcheck0->timer == TRS_V || _tcheck0->timer == TRS_LD || _tcheck0->timer == TRS_RD) {
					_tile->timer = TRS_V;
					_connected_above = true;
				}

				//check for end of rail
				_tcheck1 = LEVEL.GetTile(x - 1, y-1);
				_tcheck2 = LEVEL.GetTile(x + 1, y-1);
				if ((_tcheck0->timer == TRS_H || _tcheck0->timer == TRS_LU || _tcheck0->timer == TRS_RU) && (_tcheck1->type != TT_RAIL_TRACK || _tcheck2->type != TT_RAIL_TRACK)) {
					//no both
					if (_tcheck1->type != TT_RAIL_TRACK && _tcheck2->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_V;
					//no l
					else if (_tcheck1->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_RD;
					//no r
					else if (_tcheck2->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_LD;
					_tile->timer = TRS_V;
					_connected_above = true;
				}
			}



			//check for below tile to connect to
			_tcheck0 = LEVEL.GetTile(x, y + 1);
			if (_tcheck0->type == TT_RAIL_TRACK) {

				//easy connect, straight shot
				if (_tcheck0->timer == TRS_V || _tcheck0->timer == TRS_LU || _tcheck0->timer == TRS_RU) {
					_tile->timer = TRS_V;
					_connected_below = true;
				}

				//check for end of rail
				_tcheck1 = LEVEL.GetTile(x - 1, y + 1);
				_tcheck2 = LEVEL.GetTile(x + 1, y + 1);
				if ((_tcheck0->timer == TRS_H || _tcheck0->timer == TRS_LD || _tcheck0->timer == TRS_RD) && (_tcheck1->type != TT_RAIL_TRACK || _tcheck2->type != TT_RAIL_TRACK)) {
					//no both
					if (_tcheck1->type != TT_RAIL_TRACK && _tcheck2->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_V;
					//no l
					else if (_tcheck1->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_RU;
					//no r
					else if (_tcheck2->type != TT_RAIL_TRACK) _tcheck0->timer = TRS_LU;
					_tile->timer = TRS_V;
					_connected_below = true;
				}

			}


			//see if we need to curve

			//curve left?
			_tcheck0 = LEVEL.GetTile(x - 1, y);
			if (_tcheck0->type == TT_RAIL_TRACK) {
				if (_tcheck0->timer == TRS_H || _tcheck0->timer == TRS_RU || _tcheck0->timer == TRS_RD) {
					if (_connected_above && _connected_below) _tile->timer = TRS_CROSS;
					else {
						if (_connected_above) _tile->timer = TRS_LU;
						else if (_connected_below) _tile->timer = TRS_LD;
					}
				} else if (_tcheck0->timer == TRS_V) {
					_tcheck1 = LEVEL.GetTile(x - 1, y - 1);
					_tcheck2 = LEVEL.GetTile(x - 1, y + 1);
					//no top
					if (_tcheck1->type != TT_RAIL_TRACK || (_tcheck1->timer != TRS_V && _tcheck1->timer != TRS_RD && _tcheck1->timer != TRS_LD)) _tcheck0->timer = TRS_RD;
					//no bottom
					else if (_tcheck2->type != TT_RAIL_TRACK || (_tcheck2->timer != TRS_V && _tcheck2->timer != TRS_RU && _tcheck2->timer != TRS_LU)) _tcheck0->timer = TRS_RU;
				}
			}

			//curve right?
			_tcheck0 = LEVEL.GetTile(x + 1, y);
			if (_tcheck0->type == TT_RAIL_TRACK) {
				if (_tcheck0->timer == TRS_H || _tcheck0->timer == TRS_LU || _tcheck0->timer == TRS_LD) {
					if (_connected_above && _connected_below) _tile->timer = TRS_CROSS;
					else {
						if (_connected_above) _tile->timer = TRS_RU;
						else if (_connected_below) _tile->timer = TRS_RD;
					}
				} else if (_tcheck0->timer == TRS_V) {
					_tcheck1 = LEVEL.GetTile(x + 1, y - 1);
					_tcheck2 = LEVEL.GetTile(x + 1, y + 1);
					//no top
					if (_tcheck1->type != TT_RAIL_TRACK || (_tcheck1->timer != TRS_V && _tcheck1->timer != TRS_RD && _tcheck1->timer != TRS_LD)) _tcheck0->timer = TRS_LD;
					//no bottom
					else if (_tcheck2->type != TT_RAIL_TRACK || (_tcheck2->timer != TRS_V && _tcheck2->timer != TRS_RU && _tcheck2->timer != TRS_LU)) _tcheck0->timer = TRS_LU;
				}
			}

		break; }

		case TT_TRAIN_DEPOT:
			_tile->ref = 0;
		break;

	}
}



void HurtTile(int dmg, int x, int y, Tile* _tile, Entity* _hurtBy) {

	_tile->hp -= dmg;

	if (GAME.newsticker_time <= -(SCREEN_W - 32)) {
		if (_tile->type == TT_HQ_TL || _tile->type == TT_HQ_TR || _tile->type == TT_HQ_BL || _tile->type == TT_HQ_BR) GAME.AddNews("Your HQ is under attack at " + std::to_string(LEVEL.playerHq.origin_x) + ", " + std::to_string(LEVEL.playerHq.origin_y) + "!");
	}

	//pop
	Entity* _fx;
	_fx = SpawnFx(x * 8, y * 8, 0, 8);
	SetFxSpr(_fx, { 0, 200, 16, 16 }, { 255, 255, 255, 255 });
	_fx = SpawnFx(x * 8, y * 8, 0, 3);
	SetFxSpr(_fx, { 0, 200, 16, 16 }, { 0, 0, 0, 255 });


	if (_tile->hp <= 0) OnTileDestroy(x, y, _tile);

}



void OnTileDestroy(int x, int y, Tile* _tile, bool demolished, bool multiDestroy) {

	switch (_tile->type) {
		default:
			TileDoDefaultDestroy(x, y, _tile, demolished, multiDestroy);
		break;

		case TT_HQ_TL: case TT_HQ_TR: case TT_HQ_BL: case TT_HQ_BR: {
			_tile->type = TT_CRATER;
			if (PosIsOnScreen(x * 8, y * 8)) Sound::PlayTempSoundAt(SND_DEMOLISH, x * 8, y * 8, 0.75f, 0.8f);
			GAME.state = GS_GAMEOVER;
			break; }

		//no no no
		//case TT_CONSTRUCTION_SITE: case TT_WATER: break;
		case TT_WATER: 
			//splash
			Entity* _fx;
			for (int i = 0; i < 3; i++) {
				_fx = SpawnFx(x*8, y*8, 0, 16 + rand() % 24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
				SetFxSpr(_fx, { 16, 32, 8, 8 });
				SetFxMotion(_fx, (-10 + rand() % 30) / 10.0f, -rand() % 2, -8);
			}

			_fx = SpawnFx(x * 8, y * 8, 0, 8);
			SetFxSpr(_fx, { 8, 32, 8, 8 });
		break;

		case TT_CITYBLOCK_BIG: case TT_CITYBLOCK_SMALL: case TT_CITY_BANK:
			if (demolished) {
				LEVEL.arrCities[_tile->owner].friendliness -= 2;
				LEVEL.arrCities[_tile->owner].money = 0;
			} else {
				//warn about attack
				if (GAME.newsticker_time <= -(SCREEN_W - 32)) {
					if (LEVEL.arrCities[_tile->owner].warnAboutAttackTimer <= 0) {
						GAME.AddNews(LEVEL.arrCities[_tile->owner].name + " is under attack at " + std::to_string(x) + ", " + std::to_string(y) + "!");
						LEVEL.arrCities[_tile->owner].warnAboutAttackTimer = 1;
					}
				}
			}

			if (_tile->flags & TF_ONWATER) _tile->type = TT_WATER;
			else _tile->type = TT_CRATER;
			_tile->owner = 0;
			if (PosIsOnScreen(x * 8, y * 8)) Sound::PlayTempSound(SND_DEMOLISH, 0.2f, 1.0f);
		break;




	}
}



void TileDoDefaultDestroy(int x, int y, Tile* _tile, bool demolished, bool multiDestroy) {
	if (multiDestroy) {
		TileInfo _tileInfo = GET_TILE_INFO(_tile->type);
		if (_tileInfo.multiTiles.x != 0 || _tileInfo.multiTiles.y != 0) {
			OnTileDestroy(x + _tileInfo.multiTiles.x, y, LEVEL.GetTile(x + _tileInfo.multiTiles.x, y), demolished, false);
			OnTileDestroy(x, y + _tileInfo.multiTiles.y, LEVEL.GetTile(x, y + _tileInfo.multiTiles.y), demolished, false);
			OnTileDestroy(x + _tileInfo.multiTiles.x, y + _tileInfo.multiTiles.y, LEVEL.GetTile(x + _tileInfo.multiTiles.x, y + _tileInfo.multiTiles.y), demolished, false);
		}
	}

	if (_tile->flags & TF_ONWATER) _tile->type = TT_WATER;
	else _tile->type = TT_CRATER;
	_tile->owner = 255;
	if (PosIsOnScreen(x * 8, y * 8)) Sound::PlayTempSound(SND_DEMOLISH, 0.2f, 1.0f);
}




void City::expandTick() {
	//std::cout << timer << std::endl;

	if (warnAboutAttackTimer > 0) warnAboutAttackTimer--;

	popcount = 0;
	if (bankX != -1 && bankY!=-1) {
		Tile* _bankPos = LEVEL.GetTile(bankX, bankY);
		if (_bankPos->type != TT_CITY_BANK) {
			flags &= ~CF_HASBANK;
			bankX = -1; bankY = -1; //bank has been gooped
		}
	}
	
	//remove from active duty if out of tiles
	if (myTiles.size() <= 0) {
		
		//city is lost
		GAME.AddNews(name + " was lost to the aliens!");

		flags &= ~CF_ACTIVE;
		LEVEL.activeCities--;
	}

	for (int i = 0; i < myTiles.size(); i++) {
		Tile* _curTile = GAME.currentLevel.GetTile(myTiles[i].x, myTiles[i].y);
		if (_curTile->owner != index || !(GET_TILE_INFO(_curTile->type).flags & TIF_CITY)) {
			//check if city has been gooped by aliens
			myTiles.erase(myTiles.begin() + i);
			timer = 10;
			if (friendliness>0) friendliness-=2;
			if (maxResources > 0) maxResources--; 
			continue;
		}
		if (_curTile->type == TT_CITYBLOCK_SMALL) popcount += (4000 + (int)rand() % 999);
		else if (_curTile->type == TT_CITYBLOCK_BIG) popcount += (10000 + (int)rand() % 9999);
		else if (_curTile->type == TT_CITY_BANK) {
			popcount += (10000 + (int)rand() % 9999);
		}
	}
	
	if (resources > maxResources) resources = maxResources; // hard limit, even though its converted into money at each tick so its almost redundant.
	if (resources > 1) { //give them a little breathing room to expand still 
		if (flags&CF_HASBANK) money += 30;
		else money += 20;
		resources --;
	}

	if (money == maxMoney && flags&CF_HASBANK) {
		//send a plane to hq
		Entity* _plane = LEVEL.AddEntity(origin_x * 8, origin_y * 8, ENT_CITYPLANE);
		_plane->target_x = 8 + LEVEL.playerHq.origin_x * 8;
		_plane->target_y = 8 + LEVEL.playerHq.origin_y * 8;
		_plane->owner = index;
		_plane->ticker = money;
		money = 0;
	}

	if (timer > 0) timer--;
	else {
		//chump change regardless of resources
		money += 10;
		//time to grow
		if (resources > 0) {
			Tile* _t;
			int _placeX = origin_x;
			int _placeY = origin_y;
			int _chance;
			for (int i = 0; i < myTiles.size() * 2; i++) {
				_t = LEVEL.GetTile(_placeX, _placeY);
				if (_t->type == TT_LAND || _t->type == TT_CRATER) {
					//TileType _cityBlockType = TT_CITYBLOCK_BIG; 
					//if (i > myTiles.size()) _cityBlockType = TT_CITYBLOCK_SMALL;
					_t = BuildTileAt(_placeX, _placeY, TT_CITYBLOCK_SMALL);
					_t->owner = index;
					if (friendliness < 20) friendliness++;
					maxResources++;
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
					if (_chance > 6) {
						if (_t->type == TT_CITYBLOCK_SMALL) {
							_t = BuildTileAt(_placeX, _placeY, TT_CITYBLOCK_BIG);
							_t->owner = index;
							myTiles.push_back({ (short)_placeX, (short)_placeY });
							if (friendliness < 20) friendliness += 3;
							maxResources++;
							break;
						}
						else if (_t->type == TT_CITYBLOCK_BIG && popcount > 160000) {
							if (!(flags & CF_HASBANK)) {
								flags |= CF_HASBANK;
								_t = BuildTileAt(_placeX, _placeY, TT_CITY_BANK);
								_t->owner = index;
								myTiles.push_back({ (short)_placeX, (short)_placeY });
								if (friendliness < 20) friendliness += 5;
								maxResources++;
								bankX = _placeX;
								bankY = _placeY;
								break;
							}
						}
					}
				}
			}
		}
		timer = 4;
		/*
		std::cout << name << std::endl;
		std::cout << std::to_string(resources) << std::endl;
		std::cout << std::to_string(friendliness) << std::endl;
		std::cout << std::to_string(money) << std::endl;
		if (flags & CF_HASBANK)std::cout << "GOT A BANK" << std::endl;
		else std::cout << "no bank :(" << std::endl;
		*/
	}
}