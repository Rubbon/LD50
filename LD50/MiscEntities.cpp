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





void TrainInit(Entity* ent) {
	ent->mx = 1;
	ent->target_x = ent->x;
	ent->target_y = ent->y;
}

void TrainTick(Entity* ent) {
	if ((ent->id + GAME_TICK) % 6 == 0) {

		//move
		if (ent->x < ent->target_x) ent->x++;
		else if (ent->x > ent->target_x) ent->x --;

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
					} else
					if (_tile->timer == TRS_RD || _tile->timer == TRS_LD) {
						ent->mx = 0;
						ent->my = 1;
						_turned = true;
					}
				} else {
					_turned = false;
				}
				//}


				_tileAhead = LEVEL.GetTile((ent->x >> 3) + ent->mx, ent->y >> 3);

				//check for station to enter


				//otherwise check for rail to move onto
				if (_tileAhead->type == TT_RAIL_TRACK && _tileAhead->timer != TRS_V) {
					ent->target_x = ent->x + ent->mx * 8;
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
					} else
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
	}
}

void TrainDraw(Entity* ent) {
	Graphics::DrawSpr(TEX_CHARS, { ent->x - CAMERA_X, ent->y - 2 - CAMERA_Y, 8, 8 }, {24, 104, 8, 8});
}
