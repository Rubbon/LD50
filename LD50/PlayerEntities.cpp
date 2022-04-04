#include "PlayerEntities.h"
#include "Graphics.h"
#include "Input.h"
#include "Game.h"
#include "TileBaseInfo.h"
#include "Sound.h"
#include <algorithm>

float _spd = 0.05f;
bool muted = false;
short reserveLanding = 0;


enum PLayerJetState {
	PJS_FLYING,
	PJS_LANDING,
	PJS_LANDING_HQ,
	PJS_LANDED,
};

SDL_RendererFlip _flip = SDL_FLIP_NONE;


void PlayerJetInit(Entity* ent) {
	ent->z = -6;
	ent->fx = ent->x;
	ent->fy = ent->y;

	ent->flags |= EFL_HUMAN;
	ent->state = PJS_FLYING;
	ent->zwig = 0;

	ent->hp = 8;

}

void PlayerJetTick(Entity* ent) {

	if (Input::KeyPressed(SDL_SCANCODE_M)) {
		if (muted) {
			Game::SetMusicTo(BGM_INVASION);
			muted = false;
		}
		else {
			Game::SetMusicTo(SFX_NOSOUND);
			muted = true;
		}
	}

	switch (ent->state) {

	case PJS_FLYING: {

			ent->x = std::clamp(ent->x, 0, LEVEL_W*8);
			ent->y = std::clamp(ent->y, 0, LEVEL_H*8);

			ent->flags |= EFL_AIR;

			if (ent->z > -6) ent->z--;
			
			float _mouseAngle = atan2((CURSOR_Y + CAMERA_Y) - ent->fy, (CURSOR_X + CAMERA_X) - ent->fx);
			//lookDir = std::lerp(lookDir, _mouseAngle, 0.2f);

			//move
			if (Input::MouseHeld(MB_RIGHT)) {

				//0.3f
				if (_spd < 0.2f) _spd += 0.01f;

				ent->mx += _spd * cos(_mouseAngle);
				ent->my += _spd * sin(_mouseAngle);

			}
			else {
				if (_spd > 0.05f) _spd -= 0.01f;
			}


			//schoot
			if (Input::MouseHeld(MB_LEFT)) {
				if (ent->ticker <= 0) {
					Entity* _ent = LEVEL.AddEntity(ent->x, ent->y + 2, ENT_JETBULLET, false);
					Sound::PlayTempSoundAt(SND_BULLET,ent->x,ent->y);
					_ent->mx = 6 * cos(_mouseAngle) + ent->mx;
					_ent->my = 6 * sin(_mouseAngle) + ent->my;
					_ent->z = ent->z;
					arrEntityFuncs[_ent->entityIndex].Init(_ent);
					
					ent->ticker = 8;
				}
			}
			ent->ticker--;


			//reserve landing
			if (Input::KeyPressed(SDL_SCANCODE_SPACE)) {
				reserveLanding = 6;
			}

			//land button
			if (reserveLanding > 0) {
				TileType _tileAtFeet = LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type;

				if (_tileAtFeet >= TT_HQ_TL && _tileAtFeet <= TT_HQ_BR) {
					ent->state = PJS_LANDING_HQ;
				}

				if (GET_TILE_INFO(_tileAtFeet).flags & TIF_WALKABLE) {
					ent->state = PJS_LANDING;
				}

				//todo - also allow landing in hq
				reserveLanding = 0;
				break;
			}

			reserveLanding--;
			
			// hover wiggle like a VTOL
			if (_spd<=0.06) { 
				ent->z += sin(ent->zwig);
				if (!ent->ascending) {
					if (ent->zwig < 90) ent->zwig += 0.1;
					else ent->ascending = true;
				}
				else {
					if (ent->zwig > 0) ent->zwig -= 0.1;
					else ent->ascending = false;
				}
				//std::cout << std::to_string(ent->zwig) << std::endl;
			}
			

			ent->mx *= 0.95f;
			ent->my *= 0.95f;

			ent->fx += ent->mx;
			ent->fy += ent->my;


			ent->x = ent->fx;
			ent->y = ent->fy;



			//look at mouse
			if (CURSOR_Y + CAMERA_Y < ent->y) _flip = SDL_FLIP_VERTICAL;
			else _flip = SDL_FLIP_NONE;


			float _angle = abs(round(_mouseAngle * 180 / M_PI));

			if (_angle < 22.5) { // ->
				ent->animSpr = { 0, 88, 8, 8 };
			}
			else
			if (_angle < 50) {   // \  
				ent->animSpr = { 16, 88, 8, 8 };
			}
			else
			if (_angle < 128) {  // |
				ent->animSpr = { 32, 88, 8, 8 };
			}
			else
			if (_angle < 150) { //  /
				ent->animSpr = { 8, 88, 8, 8 };
			}
			else {				// <-
				ent->animSpr = { 40, 88, 8, 8 };
			}

		break; }

		case PJS_LANDING: case PJS_LANDING_HQ:
			if (ent->z < 0) {
				if (GAME_TICK % 3 == 0) ent->z++;

				//be in middle of tile
				if (ent->x < 4 + ((ent->x) >> 3) * 8) ent->x++;
				else if (ent->x > 4 + ((ent->x) >> 3) * 8) ent->x--;

				if (ent->y < 4 + ((ent->y) >> 3) * 8) ent->y++;
				else if (ent->y > 4 + ((ent->y) >> 3) * 8) ent->y--;

			} else {
				//finished landing
				ent->z = 0;

				if (ent->state == PJS_LANDING) {
					ent->state = PJS_LANDED;
				} else {
					GAME.jetBuildTimer = 128;
					DeleteEntity(ent);
				}

				GAME.state = GS_BUILD;
			}
		break;

		case PJS_LANDED:
			ent->flags &= ~EFL_AIR;

			//fly button
			if (Input::KeyPressed(SDL_SCANCODE_SPACE)) {
				ent->state = PJS_FLYING;
				GAME.state = GS_PLAY;
			}

		break;

	}

}

void PlayerJetDraw(Entity* ent) {

	TileType _tileAtFeet = LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type;

	//shadow
	if (GET_TILE_INFO(_tileAtFeet).flags & TIF_WALKABLE || _tileAtFeet == TT_WATER) Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - (ent->z/2) - CAMERA_X, ent->y + 1 - CAMERA_Y, 8, 3 }, { 0, 157, 8, 3 });

	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 3 + ent->z - CAMERA_Y, 8, 8 }, ent->animSpr, { 255, 255, 255, 255 }, _flip);
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, ent->animSpr, { 229, 113, 247, 255 }, _flip);
}














void JetBulletInit(Entity* ent) {
	//float _angle = atan2((CURSOR_Y + CAMERA_Y) - ent->y, (CURSOR_X + CAMERA_X) - ent->x);

	//ent->animSpr = {48 + GetSprOffsetOnAngle(_angle) * 8, 88, 8, 8};
	
	//angle
	//if (ent->my < 0) ent->animFrame = SDL_FLIP_HORIZONTAL;
	//else ent->animFrame = SDL_FLIP_NONE;
}

void JetBulletTick(Entity* ent) {
	ent->fx += ent->mx;
	ent->fy += ent->my;

	ent->x = ent->fx;
	ent->y = ent->fy;

	if (ent->y > LEVEL_H * 8 || ent->x > LEVEL_W * 8) DeleteEntity(ent);


	if (ent->wait > 20) {
		if (ent->z < 0) {
			ent->z++;
		} else {
			
			int _fxLife = 6;
			Entity* _fx;

			if (LEVEL.GetTile(ent->x >> 3, ent->y >> 3)->type == TT_WATER) {

				_fxLife = 12;

				for (int i = 0; i < 3; i++) {
					_fx = SpawnFx(ent->x, ent->y, 0, 16 + rand()%24, FXS_HAS_GRAVITY | FXS_DESTROY_ON_LAND);
					SetFxSpr(_fx, { 16, 32, 8, 8 });
					SetFxMotion(_fx,  ( - 10 + rand() % 30)/10.0f, -rand() % 2, -8);
				}

			}

			_fx = SpawnFx(ent->x, ent->y, 0, _fxLife);
			SetFxSpr(_fx, {8, 32, 8, 8});
			DeleteEntity(ent);
		}
	}

	ent->wait++;


}

void JetBulletDraw(Entity* ent) {
	//shadow
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 + (ent->z / 2) - CAMERA_X, ent->y - 4 - CAMERA_Y, 8, 8 }, {0, 24, 8, 8}, { 201, 196, 205, 255 });
	//bullet
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, {0, 32, 8, 8});
	Graphics::DrawSpr(TEX_CHARS, { ent->x - 4 - CAMERA_X, ent->y - 4 + ent->z - CAMERA_Y, 8, 8 }, {0, 24, 8, 8}, { 255, 152, 51, 255 });



}
