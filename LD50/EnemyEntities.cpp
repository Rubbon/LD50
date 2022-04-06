#include "EnemyEntities.h"
#include "Graphics.h"
#include "Game.h"
#include "TileBaseInfo.h"
#include <algorithm>



void E_UfoInit(Entity* ent) {
	ent->flags |= EFL_ALIEN | EFL_AIR;
	ent->hp = 2;
}


void E_UfoTick(Entity* ent) {
	ent->z = -6 + sin((GAME_TICK + ent->id)*0.05f) * 2;
	//std::cout << "ufo tick" << std::endl;
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
					if (_tile != NULL) {

						switch (_tile->type) {
						case TT_CITYBLOCK_BIG: case TT_CITYBLOCK_SMALL: case TT_CITY_BANK:
							//std::cout << "FOUND CITY " << std::endl;
							LEVEL.arrCities[_tile->owner].flags |= CF_FOUND;
							ent->substate = 1;
							break;
						case TT_HQ_TL: case TT_HQ_TR: case TT_HQ_BL: case TT_HQ_BR:
							//std::cout << "FOUND HQ " << std::endl;
							GAME.alienMastermind.foundHqPos = { (short)(ent->x >> 3), (short)(ent->y >> 3) };
							ent->substate = 1;
							break;
						}

						if (ent->substate != 0) break;

						//report places of interest
						if (GET_TILE_INFO(_tile->type).flags & TIF_HUMAN) {
							//std::cout << "FOUND PLACE OF INTEREST " << std::endl;
							GAME.alienMastermind.vAreasOfInterest.push_back({ (short)(ent->x >> 3), (short)(ent->y >> 3) });
						}
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
								//std::cout << "REPORTING EMPTY LAND " << std::endl;
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

				if (ent->x < -4 || ent->y < -4 || ent->x >= 4 + LEVEL_W * 8 || ent->y >= 4 + LEVEL_H * 8) DeleteEntity(ent);

				if (ent->wait <= 0) {

					//there's a tile to attack!
					if (GET_TILE_INFO(_tile->type).flags & TIF_HUMAN) {

						Entity* _fx = SpawnFx(ent->x + 2, ent->y - 4, 0, 8);
						SetFxSpr(_fx, { 0, 144, 8, 8 }, { 32, 225, 32, 255 });

						HurtTile(1, ent->x >> 3, ent->y >> 3, _tile);
						if (PosIsOnScreen(ent->x, ent->y)) Sound::PlayTempSoundAt(SND_LASER, ent->x, ent->y, 0.5f, 0.75f);
						ent->wait += 80 + rand() % 64;
						break;
					}

					//there's a unit to attack!
					Entity* _oe = GetEntityInDistFlags(ent->x, ent->y, 72, EFL_HUMAN);
					if (_oe != NULL) {

						//make bullet
						Entity* _bul = LEVEL.AddEntity(ent->x, ent->y, ENT_ALIENBULLET);
						float _angle = atan2(_oe->y - ent->y, _oe->x - _bul->x);
						_bul->mx = 3 * cos(_angle);
						_bul->my = 3 * sin(_angle);
						_bul->wait = 16;
						_bul->z = ent->z;

						if (PosIsOnScreen(ent->x, ent->y)) Sound::PlayTempSoundAt(SND_LASER, ent->x, ent->y, 0.5f, 1.5f);
						ent->wait += 32 + rand() % 48;
						break;
					}

					int _moveX = -1;
					int _moveY = -1;

					/*
					//think about moving to nearby tile
					for (int i = 0; i < 3 + rand() % 5; i++) {
						_moveX = (ent->x >> 3) + (rand() % 6) - 3;
						_moveY = (ent->y >> 3) + (rand() % 6) - 3;
						//stop trying cause we found a tile to attack
						if (GET_TILE_INFO(LEVEL.GetTile(_moveX, _moveY)->type).flags & TIF_HUMAN) break;
					}
					*/

					//look for a building to smash
					int _searchDist = 4;
					int ix, iy;
					for (ix = -_searchDist; ix < _searchDist; ix++) {
						for (iy = -_searchDist; iy < _searchDist; iy++) {
							if (GET_TILE_INFO(LEVEL.GetTile(ix + (ent->x >> 3), iy + (ent->y >> 3))->type).flags & TIF_HUMAN) {
								_moveX = ix + (ent->x >> 3);
								_moveY = iy + (ent->y >> 3);
								break;
								//std::cout << "found tile" << std::endl;
							}
						}
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
	//std::cout << "ufo tick end" << std::endl;
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

	ent->hp -= attacker->dmg;

	if (ent->hp <= 0) {

		if (attacker->entityIndex == ENT_PLAYERJET) GAME.playerCash += 10;

		//die fx
		for (int i = 0; i < 4; i++) {
			_fx = SpawnFx(ent->x, ent->y - 4, ent->z, 16 + rand() % 24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
			SetFxSpr(_fx, { 8, 0, 8, 8 }, { 255, 64, 64 });
			SetFxMotion(_fx, (-10 + rand() % 30) / 10.0f, -rand() % 2, -8);
		}

		//pop
		_fx = SpawnFx(ent->x, ent->y - 8, ent->z, 8);
		SetFxSpr(_fx, { 0, 200, 16, 16 }, { 255, 255, 255, 255 });
		_fx = SpawnFx(ent->x, ent->y - 8, ent->z, 3);
		SetFxSpr(_fx, { 0, 200, 16, 16 }, { 0, 0, 0, 255 });


		//if recon, alert motherbase
		if (ent->entityIndex == ENT_UFO && ent->state == ES_RECON && rand()%3 == 0) {
			//std::cout << "ADDED DEATH SPOT AS AREA OF INTEREST " << std::endl;
			GAME.alienMastermind.vAreasOfInterest.push_back({ (short)(ent->x >> 3), (short)(ent->y >> 3) });
		}

		if (PosIsOnScreen(ent->x, ent->y)) Sound::PlayTempSoundAt(SND_DEMOLISH, ent->x, ent->y, 1.0f, 3.0f);
		DeleteEntity(ent);
	}


	//hit fx
	for (int i = 0; i < 2; i++) {
		_fx = SpawnFx(ent->x, ent->y - 4, ent->z, 16 + rand() % 24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
		SetFxSpr(_fx, { 8, 0, 8, 8 }, { 255, 64, 64, 255 });
		SetFxMotion(_fx, (-10 + rand() % 30) / 10.0f, -rand() % 2, -8);
	}

	//if attacker, stop to attack back
	if (ent->entityIndex != ENT_WALKER) {
		if (ent->state == ES_ATTACKER) {
			ent->target_x = ent->x;
			ent->target_y = ent->y;
		}
	}

	if (PosIsOnScreen(ent->x, ent->y)) Sound::PlayTempSoundAt(SND_PLACE_BUILDING, ent->x, ent->y, 0.5f, 3.0f);

}








const SDL_Rect arrAnimWalkerWalk[4] = { {0, 179, 9, 10}, {9, 179, 7, 10}, {0, 179, 9, 10},  {17, 179, 7, 10} };


void E_WalkerInit(Entity* ent) {
	ent->flags |= EFL_ALIEN;
	ent->target_x = ent->x >> 3;
	ent->target_y = ent->y >> 3;

	ent->hp = 14;

}


void E_WalkerTick(Entity* ent) {
	//std::cout << "walker tick" << std::endl;
	int _tx = ent->x >> 3;
	int _ty = ent->y >> 3;

	Tile* _t = LEVEL.GetTile(_tx, _ty);

	int _timer = 2;
	if (_t->type == TT_WATER) _timer = 3;

	if (GAME_TICK % _timer == 0) {

		if (ent->state == 0) {
			ent->z = 0;

			//we ain't moving - check some stuff
			if (_tx == ent->target_x && _ty == ent->target_y) {

				if (ent->x < -4 || ent->y < -4 || ent->x >= 4 + LEVEL_W * 8 || ent->y >= 4 + LEVEL_H * 8) DeleteEntity(ent);

				ent->animFrame = 0;

				if (ent->wait <= 0) {

					//look for a building to smash
					int _searchDist = 8;
					int ix, iy;
					bool _foundTile = false;
					for (ix = -_searchDist; ix < _searchDist; ix++) {
						for (iy = -_searchDist; iy < _searchDist; iy++) {
							if (GET_TILE_INFO(LEVEL.GetTile(ix + _tx, iy + _ty)->type).flags & TIF_HUMAN) {
								ent->target_x = ix + _tx;
								ent->target_y = iy + _ty;
								_foundTile = true;
								break;
							}
						}
					}

					if (!_foundTile) {
						ent->target_x = _tx + (rand() % 6) - 3;
						ent->target_y = _ty + (rand() % 6) - 3;
					}

					ent->wait = 64 + rand() % 32;
				} else {
					ent->wait--;
				}

				// sink in water
				if (_t->type == TT_WATER) {
					ent->z = 4;
				}

			}
			else {
				//move to target pos
				if (ent->x < ent->target_x * 8) {
					ent->x++;
				}
				else if (ent->x > ent->target_x * 8) {
					ent->x--;
				}

				if (ent->y < ent->target_y * 8) {
					ent->y++;
				}
				else if (ent->y > ent->target_y * 8) {
					ent->y--;
				}


				if (!(GET_TILE_INFO(_t->type).flags & TIF_WALKABLE)) {
					if (_t->type == TT_WATER) {
						ent->z = 4;
					} else {
						ent->z = 0;
						ent->state = 1;
						ent->substate = 0;
					}
				}

				if (GAME_TICK % 16 == 0) {
					ent->animFrame++;
					if (ent->animFrame > 3) ent->animFrame = 0;
				}

			}

		} else {
			ent->animFrame = 0;

			//attack
			if (ent->substate < 0) ent->substate++;

			if (ent->substate == 0) {
				ent->z--;
				if (ent->z <= -8) ent->substate++;
			} else
			if (ent->substate == 1) {
				ent->z ++;

				if (ent->z >= 0) {
					ent->z = 0;
					ent->substate = -16;

					if (PosIsOnScreen(ent->x, ent->y))  Sound::PlayTempSound(SND_DEMOLISH, 0.3f, 1.25f);

					//hurt the tile
					HurtTile(1, _tx, _ty, _t, ent);

					//done
					if (GET_TILE_INFO(_t->type).flags & TIF_WALKABLE) {
						ent->state = 0;
					}
				}

			}


		}

	}

}


void E_WalkerDraw(Entity* ent) {

	int _cropY = 0;
	if (ent->z > 0) _cropY = ent->z;

	//shadow
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 1 - CAMERA_Y, 8, 2 }, { 24, 187, 8, 2 });
	//me
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 1 - (arrAnimWalkerWalk[ent->animFrame].w / 2) - CAMERA_X, ent->y - 10 + ent->z - CAMERA_Y, arrAnimWalkerWalk[ent->animFrame].w, arrAnimWalkerWalk[ent->animFrame].h - _cropY }, 
						{ arrAnimWalkerWalk[ent->animFrame].x, arrAnimWalkerWalk[ent->animFrame].y, arrAnimWalkerWalk[ent->animFrame].w, arrAnimWalkerWalk[ent->animFrame].h - _cropY });
}











void E_AlienUfoBulletTick(Entity* ent) {
	ent->fx += ent->mx;
	ent->fy += ent->my;

	ent->x = ent->fx;
	ent->y = ent->fy;

	if (ent->y >= LEVEL_H * 8 || ent->x >= LEVEL_W * 8) DeleteEntity(ent);

	if (ent->ticker > ent->wait) {
		if (ent->z >= 0) DeleteEntity(ent);
		else ent->z += 1;
	} else {
		ent->ticker++;
	}

	//check for hitting enemy
	Entity* _ent = GetEntityInDistFlags(ent->x, ent->y, 5, EFL_HUMAN);
	//Entity* _ent = GetEntityInTileFlags(ent->x, ent->y, EFL_HUMAN);
	if (_ent != NULL) {
		_ent->hp -= ent->dmg;
		arrEntityFuncs[_ent->entityIndex].OnHurt(_ent, ent);
		DeleteEntity(ent);
	}


}

void E_AlienUfoBulletDraw(Entity* ent) {

	SDL_Colour _c1, _c2;

	if (GAME_TICK % 20 == 0) {
		_c1 = {255, 255, 255, 255};
		_c2 = {32, 216, 32, 255};
	} else {
		_c1 = { 32, 216, 32, 255 };
		_c2 = { 255, 255, 255, 255 };
	}

	//shadow
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 + (ent->z / 2) - CAMERA_X, ent->y - 4 - CAMERA_Y, 8, 8 }, { 0, 24, 8, 8 }, { 201, 196, 205, 255 });
	//bullet
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, { 0, 32, 8, 8 }, _c1);
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, { 0, 24, 8, 8 }, _c2);
}














void E_HunterInit(Entity* ent) {
	ent->hp = 8;
	ent->z = -6;
	ent->flags |= EFL_ALIEN | EFL_AIR;
}


void E_HunterTick(Entity* ent) {
	//ent->z += sin(GAME_TICK * 0.1f);
	//std::cout << "hunter tick" << std::endl;

	//calc target
	if ((ent->id + GAME_TICK) % 24 == 0) {
		Entity* _oe = GetEntityInDistFlags(ent->x, ent->y, 160, EFL_HUMAN);
		if (_oe != NULL) {
			ent->target_x = _oe->x;
			ent->target_y = _oe->y;
		} else {
			ent->target_x = 128 + rand() % ((LEVEL_W * 8) - 256);
			ent->target_y = 128 + rand() % ((LEVEL_H * 8) - 256);
		}
	}

	float _angle = atan2(ent->target_x - ent->fy, ent->target_y - ent->fx);
	ent->mx += 0.1f * cos(_angle);
	ent->my += 0.1f * sin(_angle);


	//consider shootin'
	/*
	if (GAME_TICK % 16 == 0) {
		if (ent->wait > 0) {

			if (GetEntityInDistFlags(ent->x, ent->y, 128, EFL_HUMAN) != NULL) {
				//make bullet
				Entity* _bul = LEVEL.AddEntity(ent->x, ent->y, ENT_ALIENBULLET);
				_bul->mx = 3 * cos(_angle);
				_bul->my = 3 * sin(_angle);
				_bul->wait = 40;
				_bul->z = ent->z;
			}

			ent->wait--;
		}
	}

	if ((ent->id + GAME_TICK) % 80 == 0) {
		ent->wait = 10;
	}
	*/

	if (ent->wait <= 0) {
		if (GetEntityInDistFlags(ent->x, ent->y, 128, EFL_HUMAN) != NULL) {
			Entity* _bul;

			if (PosIsOnScreen(ent->x, ent->y)) Sound::PlayTempSoundAt(SND_LASER, ent->x, ent->y, 0.5f, 0.75f);

			if (ent->ticker == 0) ent->ticker = 1;
			else ent->ticker = 0;


			_bul = LEVEL.AddEntity(ent->x, ent->y + 4, ENT_ALIENBULLET);
			_bul->mx = -2;
			if (ent->ticker == 1) _bul->my = -2;
			_bul->z = ent->z;
			_bul->wait = 20;

			_bul = LEVEL.AddEntity(ent->x, ent->y + 4, ENT_ALIENBULLET);
			_bul->mx = 2;
			if (ent->ticker == 1) _bul->my = -2;
			_bul->z = ent->z;
			_bul->wait = 20;


			_bul = LEVEL.AddEntity(ent->x, ent->y + 4, ENT_ALIENBULLET);
			_bul->my = -2;
			if (ent->ticker == 1) _bul->mx = -2;
			_bul->z = ent->z;
			_bul->wait = 20;


			_bul = LEVEL.AddEntity(ent->x, ent->y + 4, ENT_ALIENBULLET);
			_bul->my = 2;
			if (ent->ticker == 1) _bul->mx = 2;
			_bul->z = ent->z;
			_bul->wait = 20;

			ent->wait = 40;
		}
	} else {
		ent->wait--;
	}

	ent->mx *= 0.95f;
	ent->my *= 0.95f;

	ent->fx += ent->mx;
	ent->fy += ent->my;

	ent->x = ent->fx;
	ent->y = ent->fy;

}


void E_HunterDraw(Entity* ent) {
	TileType _tileAtFeet = LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type;

	//shadow
	if (GET_TILE_INFO(_tileAtFeet).flags & TIF_WALKABLE || _tileAtFeet == TT_WATER) Graphics::DrawSpr(TEX_CHARS, { ent->x - 6 - (ent->z / 2) - CAMERA_X, ent->y + 3 - CAMERA_Y, 16, 5 }, { 0, 172, 16, 5 });


	//update angle
	//if ((ent->id + GAME_TICK) % 10 == 0) {
		//float _angle = atan2((float)ent->target_y - ent->y, (float)ent->target_x - ent->x);
		//ent->animSpr = { GetSprOffsetOnAngle(_angle) * 8, 192, 8, 8 };
		//ent->animSpr = { GetSprOffsetOnAngle(_angle) * 8, 192, 8, 8 };
	//}

	//flip
	//SDL_RendererFlip _flip = SDL_FLIP_NONE;
	//if (ent->target_y < ent->y) _flip = SDL_FLIP_VERTICAL;

	//Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 3 + ent->z - CAMERA_Y, 8, 8 }, ent->animSpr, { 255, 255, 255, 255 }, _flip);
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 16, 12 }, { ((GAME_TICK / 4) % 2) * 16, 160, 16, 12}, { 255, 255, 255, 255 });
}


