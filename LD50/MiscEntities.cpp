#include "MiscEntities.h"
#include "Graphics.h"
#include "Game.h"
#include "TileBaseInfo.h"




void FxInit(Entity* ent) {
	ent->mz = -3;
}

void FxTick(Entity* ent) {
	
	//die after timer
	ent->hp--;
	if (ent->hp <= 0) DeleteEntity(ent);



	if (ent->z >= 0) {
		if (ent->flags & FXS_DESTROY_ON_LAND) DeleteEntity(ent);
	} else {
		if (ent->flags & FXS_HAS_GRAVITY) {
			if (ent->mz < 3) ent->mz += 0.25f;
		}
	}


	//move
	ent->fx += ent->mx;
	ent->fy += ent->my;
	ent->z += ent->mz;

	ent->x = ent->fx;
	ent->y = ent->fy;

}

void FxDraw(Entity* ent) {
	Graphics::DrawSpr(TEX_CHARS, { ent->x - CAMERA_X - ent->animSpr.w / 2, ent->y - CAMERA_Y, ent->animSpr.w, ent->animSpr.h }, ent->animSpr, ent->blend);
}








//ticker - cargo load
//substate - whether on statioin or not
//mx - my - movmenet
//fy - people to move to city spot


void TrainInit(Entity* ent) {
	ent->mx = 1;
	ent->target_x = ent->x;
	ent->target_y = ent->y;
	ent->fy = 0;

	Tile* _tile = LEVEL.GetTile(ent->x >> 3, ent->y >> 3);
	if (_tile->timer == TRS_V) {
		ent->mx = 0;
		ent->my = 1;
	}

}

void TrainTick(Entity* ent) {
	if ((ent->id + GAME_TICK) % 6 == 0) {
		if (ent->wait <= 0) {

			//move
			if (ent->x < ent->target_x) ent->x++;
			else if (ent->x > ent->target_x) ent->x--;

			if (ent->y < ent->target_y) ent->y++;
			else if (ent->y > ent->target_y) ent->y--;

			//we're at the tile we want to be at
			if (ent->x == ent->target_x && ent->y == ent->target_y) {

				Tile* _tile = LEVEL.GetTile(ent->x >> 3, ent->y >> 3);
				Tile* _tileAhead;

				bool _turned = false;


				if (ent->mx != 0) {
					//move

				lStartMovement:

					//see if we need to go up/down
					//substate is 0 if first entering tile
					//if (ent->substate == 0) {
					if (!_turned) {
						if (_tile->timer == TRS_RU || _tile->timer == TRS_LU) {
							ent->mx = 0;
							ent->my = -1;
							_turned = true;
						}
						else
							if (_tile->timer == TRS_RD || _tile->timer == TRS_LD) {
								ent->mx = 0;
								ent->my = 1;
								_turned = true;
							}
					}
					else {
						_turned = false;
					}
					//}


					_tileAhead = LEVEL.GetTile((ent->x >> 3) + ent->mx, ent->y >> 3);


					//otherwise check for rail to move onto
					if (_tileAhead->type == TT_RAIL_STATION_H || (_tileAhead->type == TT_RAIL_TRACK && _tileAhead->timer != TRS_V)) {
						ent->target_x = ent->x + ent->mx * 8;
					}

					//check if we're at a station
					if (ent->substate != 1 && _tile->type == TT_RAIL_STATION_H) {
						ent->mx = -ent->mx;
						ent->wait = 8;
						ent->substate = 1;

						//we're at a station, do the deed
						//check for factory or city
						bool _foundSomething = false;

						Tile* _t;
						Entity* _fx;

						for (int ix = -1; ix < 2; ix++) {
							for (int iy = -1; iy < 2; iy++) {
								_t = LEVEL.GetTile((ent->x >> 3) + ix, (ent->y >> 3) + iy);

								if (_t->type == TT_FACTORY_TL || _t->type == TT_FACTORY_TR || _t->type == TT_FACTORY_BL || _t->type == TT_FACTORY_BR) {
									ent->ticker++;
									_foundSomething = true;

									_fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
									SetFxSpr(_fx, { 0, 48, 8, 8 }, { 0, 255, 255, 255 });
									SetFxMotion(_fx, 0, -0.15f, 0);

									break;
								}

								if (_t->type == TT_CITYBLOCK_BIG || _t->type == TT_CITYBLOCK_SMALL || _t->type == TT_CITY_BANK) {
									_foundSomething = true;

									if (ent->ticker > 0) {
										LEVEL.arrCities[_t->owner].resources += ent->ticker;
										ent->ticker = 0;

										_fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
										SetFxSpr(_fx, { 0, 56, 8, 8 }, { 0, 255, 255, 255 });
										SetFxMotion(_fx, 0, -0.15f, 0);
									} else {
										//pick people up instead
										if (LEVEL.arrCities[_t->owner].myTiles.size() > 12 && ent->fy < 16) {
											ent->fy += rand() % 16;
											LEVEL.arrCities[_t->owner].friendliness += 1;
											_fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
											SetFxSpr(_fx, { 0, 48, 8, 8 }, { 0, 255, 255, 255 });
											SetFxMotion(_fx, 0, -0.15f, 0);
										}

									}
									break;
								}

								if (_t->type == TT_CITYSTARTER) {
									_foundSomething = true;

									//add people to citystarter
									if (ent->fy > 0) {
										int _amt = rand() % (int)ent->fy;
										ent->fy -= _amt;

										_t->timer += ent->fy;

										_fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
										SetFxSpr(_fx, { 0, 56, 8, 8 }, { 0, 255, 255, 255 });
										SetFxMotion(_fx, 0, -0.15f, 0);

										//see if we can turn it into a city
										if (_t->timer > 324 + (rand() % 64) - 32) {
											_t->timer = 0;

											for (int i = 0; i < MAX_CITIES; i++) {
												if (!(LEVEL.arrCities[i].flags & CF_ACTIVE)) {

													LEVEL.arrCities[i].flags = CF_ACTIVE;
													LEVEL.arrCities[i].origin_x = (ent->x >> 3) + ix;
													LEVEL.arrCities[i].origin_y = (ent->y >> 3) + iy;
													LEVEL.arrCities[i].name = LevelGenerator::GetCityName();
													LEVEL.arrCities[i].index = i;
													LEVEL.arrCities[i].myTiles.clear();

													Tile* _tile = BuildTileAt((ent->x >> 3) + ix, (ent->y >> 3) + iy, TT_CITYBLOCK_SMALL);
													_tile->owner = i;
													LEVEL.arrCities[i].myTiles.push_back({ (short)((ent->x >> 3) + ix), (short)((ent->y >> 3) + iy) });

												}
											}							
										}

									}

									break;
								}

							}
						}

						//otherwise do a ?
						if (!_foundSomething) {
							_fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
							SetFxSpr(_fx, { 0, 40, 8, 8 }, {0, 255, 255, 255});
							SetFxMotion(_fx, 0, -0.15f, 0);
						}

						return;
					} else {
						ent->substate = 0;
					}

				}


				if (ent->my != 0) {
					//move

					if (!_turned) {
						if (_tile->timer == TRS_RU || _tile->timer == TRS_RD) {
							ent->mx = 1;
							ent->my = 0;
							_turned = true;
							goto lStartMovement;
						}
						else
							if (_tile->timer == TRS_LU || _tile->timer == TRS_LD) {
								ent->mx = -1;
								ent->my = 0;
								_turned = true;
								goto lStartMovement;
							}
					}

					_tileAhead = LEVEL.GetTile(ent->x >> 3, (ent->y >> 3) + ent->my);


					//check for rail to move onto
					if (_tileAhead->type == TT_RAIL_TRACK && _tileAhead->timer != TRS_H) {
						ent->target_y = ent->y + ent->my * 8;
					}

				}

				//fallen off rail, die
				if (!(GET_TILE_INFO(_tile->type).flags & TIF_RAIL)) {
					DeleteEntity(ent);
				}

			}
		} else {
			ent->wait--;
		}
	}
}

void TrainDraw(Entity* ent) {
	Graphics::DrawSpr(TEX_CHARS, { ent->x - CAMERA_X, ent->y - 2 - CAMERA_Y, 8, 8 }, {24, 104, 8, 8});
}







#include "Input.h"


void CityPlaneInit(Entity* ent) {
	
}

void CityPlaneTick(Entity* ent) {

	//ent->z += sin((GAME_TICK + ent->id) * 0.05f) * 0.01f;

	if (ent->wait <= 0) {

		switch (ent->state) {
		case 0: // fly to target
			if (ent->z > -5) ent->z--;

			//move to target pos
			if (ent->x < ent->target_x) ent->x++;
			else if (ent->x > ent->target_x) ent->x--;

			if (ent->y < ent->target_y) ent->y++;
			else if (ent->y > ent->target_y) ent->y--;

			if (ent->x == ent->target_x && ent->y == ent->target_y) ent->state = 1;

			break;

		case 1: // land
			if (ent->z < 0) {
				ent->z++;
			} else if (ent->substate == 0) {
				//land at players place

				//fx
				Entity* _fx = SpawnFx(ent->x + 4, ent->y - 12, ent->z - 2, 32);
				SetFxSpr(_fx, { 0, 64, 8, 8 }, { 0, 255, 255, 255 });
				SetFxMotion(_fx, 0, -0.15f, 0);

				//give me money
				GAME.playerCash += ent->ticker;

				ent->state = 0;
				ent->target_x = LEVEL.arrCities[ent->owner].origin_x * 8;
				ent->target_y = LEVEL.arrCities[ent->owner].origin_y * 8;
				ent->substate = 1;
				ent->wait = 32;
			} else {
				//kill
				DeleteEntity(ent);
			}

			break;
		}

	} else {
		ent->wait--;
	}


}

void CityPlaneDraw(Entity* ent) {
	TileType _tileAtFeet = LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type;

	//shadow
	if (GET_TILE_INFO(_tileAtFeet).flags & TIF_WALKABLE || _tileAtFeet == TT_WATER) Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - (ent->z / 2) - CAMERA_X, ent->y + 1 - CAMERA_Y, 8, 3 }, { 0, 157, 8, 3 });


	//update angle
	if ((ent->id + GAME_TICK) % 10 == 0) {
		float _angle = atan2((float)ent->target_y - ent->y, (float)ent->target_x - ent->x);
		ent->animSpr = { GetSprOffsetOnAngle(_angle) * 8, 88, 8, 8 };
	}

	//flip
	SDL_RendererFlip _flip = SDL_FLIP_NONE;
	if (ent->target_y < ent->y) _flip = SDL_FLIP_VERTICAL;

	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 3 + ent->z - CAMERA_Y, 8, 8 }, ent->animSpr, { 255, 255, 255, 255 }, _flip);
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, ent->animSpr, { 104, 170, 255, 255 }, _flip);
}
