#include "EnemyEntities.h"
#include "Graphics.h"
#include "Game.h"
#include "TileBaseInfo.h"
#include <algorithm>



void E_UfoInit(Entity* ent) {
	ent->flags |= EFL_ALIEN;
	ent->hp = 3;
}


void E_UfoTick(Entity* ent) {

	ent->z = -6 + sin((GAME_TICK + ent->id)*0.05f) * 2;

	//move to target pos
	if (ent->x < ent->target_x) ent->x++;
	else if (ent->x > ent->target_x) ent->x--;

	if (ent->y < ent->target_y) ent->y++;
	else if (ent->y > ent->target_y) ent->y--;

	switch (ent->state) {
		case ES_RECON:
			if (ent->substate != 2 && ent->x == ent->target_x && ent->y == ent->target_y) {

				//report what we see
				if (ent->wait == 0 && ent->substate == 0) {
					Tile* _tile = LEVEL.GetTile(ent->x >> 3, ent->y >> 3);

					switch (_tile->type) {
						case TT_CITYBLOCK_BIG: case TT_CITYBLOCK_SMALL: case TT_CITY_BANK:
							std::cout << "FOUND CITY " << std::endl;
							LEVEL.arrCities[_tile->owner].flags |= CF_FOUND;
							ent->substate = 1;
						break;
						case TT_HQ_TL: case TT_HQ_TR: case TT_HQ_BL: case TT_HQ_BR:
							std::cout << "FOUND HQ " << std::endl;
							GAME.alienMastermind.foundHqPos = {(short)(ent->x >> 3), (short)(ent->y >> 3)};
							ent->substate = 1;
						break;
					}

					if (ent->substate != 0) break;

					//report places of interest
					if (GET_TILE_INFO(_tile->type).flags & TIF_HUMAN) {
						std::cout << "FOUND PLACE OF INTEREST " << std::endl;
						GAME.alienMastermind.vAreasOfInterest.push_back({(short)(ent->x >> 3), (short)(ent->y >> 3)});
					}


				}


				ent->wait++;

				if (ent->wait >= 16) {
					ent->wait = 0;
					ent->ticker++;

					int _xDir = 1;
					if (ent->ticker / RECON_SPACE_SIZE % 2 != 0) _xDir = -1;
				
					//move
					if (ent->ticker % RECON_SPACE_SIZE == 0) ent->target_y += 8;
					else  ent->target_x += 8 * _xDir;
					
					//we're done searching
					if (ent->ticker >= RECON_SPACE_SIZE * RECON_SPACE_SIZE) {

						//consider reporting as empty plot if nothing was found
						if (ent->substate == 0) {
							if (rand() % 3 == 0) {
								std::cout << "REPORTING EMPTY LAND " << std::endl;
								GAME.alienMastermind.vEmptyPlots.push_back({ (short)(ent->x >> 3), (short)(ent->y >> 3) });
							}
						}

						ent->substate = 2;
					}

				}

			}
		break;

		case ES_ATTACKER:
			
			Tile* _tile = LEVEL.GetTile(ent->x >> 3, ent->y >> 3);

			//if we're at destination, consider moving around a little - or shoot if something is near
			if (ent->x == ent->target_x && ent->y == ent->target_y) {

				if (ent->wait <= 0) {

					//there's a unit to attack!


					//there's a tile to attack!
					if (GET_TILE_INFO(_tile->type).flags & TIF_HUMAN) {

						Entity* _fx = SpawnFx(ent->x + 2, ent->y - 4, 0, 8);
						SetFxSpr(_fx, { 0, 144, 8, 8 }, {32, 225, 32, 255});

						HurtTile(1, ent->x >> 3, ent->y >> 3, _tile);
						Sound::PlayTempSoundAt(SND_LASER, ent->x, ent->y);
						ent->wait += 80 + rand() % 64;
						break;
					}

					int _moveX = -1;
					int _moveY = -1;

					//think about moving to nearby tile
					for (int i = 0; i < 3 + rand() % 5; i++) {
						_moveX = (ent->x >> 3) + (rand() % 6) - 3;
						_moveY = (ent->y >> 3) + (rand() % 6) - 3;
						//stop trying cause we found a tile to attack
						if (GET_TILE_INFO(LEVEL.GetTile(_moveX, _moveY)->type).flags & TIF_HUMAN) break;
					}

					//think about moving randomly
					if (_moveX == -1) {
						_moveX = (ent->x >> 3) + (rand() % 5) - 2;
						_moveY = (ent->y >> 3) + (rand() % 5) - 2;
					}

					if (LEVEL.GetEntityAtTile(_moveX, _moveY, EFL_AIR) == NULL) {

						ent->target_x = std::clamp((int)ent->target_x, 0, LEVEL_W*8);
						ent->target_y = std::clamp((int)ent->target_y, 0, LEVEL_H*8);

						ent->target_x = _moveX*8;
						ent->target_y = _moveY*8;
						ent->wait += 128 + rand() % 128;
					}
				}

				ent->wait--;

			}
			



		break;

	}
}


void E_UfoDraw(Entity* ent) {

	TileType _tileAtFeet = LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type;

	//shadow
	if (_tileAtFeet == TT_LAND || _tileAtFeet == TT_WATER) Graphics::DrawSpr(TEX_CHARS, {ent->x - 6 - ent->z - CAMERA_X, ent->y + 1 - CAMERA_Y, 8, 3 }, {0, 157, 8, 3});
	//me
	Graphics::DrawSpr(TEX_CHARS, {ent->x - 4 -CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 5 }, {0, 152, 8, 5});
}




void E_UfoHurt(Entity* ent, Entity* attacker) {

	Entity* _fx;

	if (ent->hp <= 0) {

		//die fx
		for (int i = 0; i < 4; i++) {
			_fx = SpawnFx(ent->x, ent->y, 0, 16 + rand() % 24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
			SetFxSpr(_fx, { 8, 0, 8, 8 }, { 255, 64, 64 });
			SetFxMotion(_fx, (-10 + rand() % 30) / 10.0f, -rand() % 2, -8);
		}

		DeleteEntity(ent);
	}


	//hit fx
	for (int i = 0; i < 2; i++) {
		_fx = SpawnFx(ent->x, ent->y, 0, 16 + rand() % 24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
		SetFxSpr(_fx, { 8, 0, 8, 8 }, { 255, 64, 64, 255 });
		SetFxMotion(_fx, (-10 + rand() % 30) / 10.0f, -rand() % 2, -8);
	}


}








const SDL_Rect arrAnimWalkerWalk[4] = { {0, 179, 9, 10}, {9, 179, 7, 10}, {0, 179, 9, 10},  {17, 179, 7, 10} };


void E_WalkerInit(Entity* ent) {
	ent->flags |= EFL_ALIEN;
}


void E_WalkerTick(Entity* ent) {
	if (GAME_TICK % 16 == 0) {
		ent->animFrame++;
		if (ent->animFrame > 3) ent->animFrame = 0;
	}
}


void E_WalkerDraw(Entity* ent) {
	//shadow
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 1 - CAMERA_Y, 8, 2 }, { 24, 187, 8, 2 });
	//me
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 1 - arrAnimWalkerWalk[ent->animFrame].w/2 - CAMERA_X, ent->y - 10 + ent->z - CAMERA_Y, arrAnimWalkerWalk[ent->animFrame].w, arrAnimWalkerWalk[ent->animFrame].h}, arrAnimWalkerWalk[ent->animFrame]);
}


