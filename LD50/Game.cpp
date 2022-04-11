#include "Game.h"
#include "Input.h"
#include <string>
#include <algorithm>
#include "TileBaseInfo.h"

bool RUN_GAME = false;
Game GAME = {};


void Game::Init() {
	
	LevelGenerator _gen = {};

	_gen.GenerateWorld(&currentLevel);

	CAMERA_X = (LEVEL_W * 4) - SCREEN_W / 2;
	CAMERA_Y = (LEVEL_H * 4) - SCREEN_H / 2;


	state = GS_BUILD_HQ;
	tileToBuild = TT_HQ_TL;


	//music thing
	//bgm audio source
	audio_bgm.Generate(false);
	audio_bgm.SetLooping(true);
	audio_bgm.SetGain(0.5f);

	
	SetMusicTo(BGM_BUILDMODE);
}



int _borderW;

//temp (for screen drag)
bool draggingCam = false;
int camDragX, camDragY;

int _freeCashTick = 0;

void Game::Tick() {
	//reset cursor state first thing
	cursorState = CS_POINTER;

	//mute butotn
	if (Input::KeyPressed(SDL_SCANCODE_M)) {
		mute_music = !mute_music;

		if (mute_music == true) {
			audio_bgm.soundBufferID = 0;
			audio_bgm.Stop();
		} else {
			if (state == GS_PLAY) SetMusicTo(BGM_INVASION);
			else SetMusicTo(BGM_BUILDMODE);
		}
	}

	//no tick
	if (state == GS_GAMEOVER) {
		SetMusicTo(SFX_NOSOUND);
		return;
	}

	//get mone
	if (_freeCashTick <= 0) {
		playerCash++;
		_freeCashTick = 256;
	}
	else {
		_freeCashTick--;
	}

	//fix border offset
	_borderW = std::max(((SCREEN_W / 20) >> 3) * 8, 16);

	gameTick++;

	currentLevel.Tick();

	mouseInMenu = 0;

	//see if we're at HQ
	playerIsAtHQ = ((playerJet == NULL || playerJet->flags & EFL_DELETED) && LEVEL.playerHq.flags & CF_ACTIVE);


	//see if we're hovering over the screen borders
	if (CURSOR_X < _borderW || CURSOR_X > SCREEN_W - _borderW || CURSOR_Y < borderTopSize || CURSOR_Y > SCREEN_H - borderBottomSize) {
		mouseInMenu = 1;
	}

	
	hovered_tile_x = (CURSOR_X + CAMERA_X) >> 3;
	hovered_tile_y = (CURSOR_Y + CAMERA_Y) >> 3;


	//controlling jet mode
	if (state == GS_PLAY) {
		if (playerJet != NULL && !(playerJet->flags & EFL_DELETED)) {
			CAMERA_X = playerJet->mx + playerJet->x - SCREEN_W/2;
			CAMERA_Y = playerJet->my + playerJet->y - SCREEN_H/2;
		}

		cursorState = CS_CROSSHAIR;

	}


	//build menu
	if (state == GS_BUILD) {

		bm_hover = -1;

		//fix offsets
		bm_startX = SCREEN_W - 16 - (BUILD_OPTIONS * 24);


		//HOVER OVER TILES
		if (CURSOR_X >= bm_startX && CURSOR_Y >= SCREEN_H - 24) {

			mouseInMenu = 1;

			//hover
			bm_hover = (CURSOR_X - bm_startX) / 24;

			//select/deselect
			if (Input::MousePressed(MB_LEFT)) {
				if (bm_selected_opt == bm_hover) bm_selected_opt = -1;
				else bm_selected_opt = bm_hover;
			}

		}

		//stop building tile
		if (Input::MousePressed(MB_RIGHT)) bm_selected_opt = -1;


		//building tile
		if (bm_selected_opt != -1) {
			tileToBuild = (TileType)arrBuildOptions[bm_selected_opt];
		} else {
			tileToBuild = TT_NONE;
		}


		//enter jet if at hq
		if (playerIsAtHQ) {
			if (jetBuildTimer <= 0) {
				if (Input::KeyPressed(SDL_SCANCODE_SPACE)) {
					//enter jet
					playerJet = LEVEL.AddEntity((LEVEL.playerHq.origin_x * 8) + 8, (LEVEL.playerHq.origin_y * 8) + 8, ENT_PLAYERJET);
					playerJet->my = -1;
					playerIsAtHQ = false;
					state = GS_PLAY;
					Sound::PlayTempSoundAt(SND_TAKEOFF, playerJet->x, playerJet->y, 0.5f);

					SetMusicTo(BGM_INVASION);

					//reset build options
					bm_selected_opt = -1;
					tileToBuild = TT_NONE;

				}
			} else {
				//decrement build timer
				jetBuildTimer--;
			}

			//repair HQ
			//if (GAME_TICK % 60 == 0) {
				//Tile* _t = LEVEL.GetTile(LEVEL.playerHq.origin_x, LEVEL.playerHq.origin_y);
				//if (_t->hp < GET_TILE_INFO(_t->type).baseHp) _t->hp++;
			//}

		}

	}



	if (state == GS_BUILD || state == GS_BUILD_HQ) {

		//building tile
		if ((tileToBuild != TT_NONE || bm_selected_opt != -1) && mouseInMenu == 0) {
			cursorState = CS_BUILD_TILE;

			if (Input::MousePressed(MB_LEFT)) {

				if (tileToBuild != TT_NONE) {
					//build
					if (CheckIfCanBuildTile(hovered_tile_x, hovered_tile_y, tileToBuild)) {

						playerCash -= GET_TILE_INFO(tileToBuild).buildCost;

						int _sp = 0;
						//special case for stuyff on water
						if (LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type == TT_WATER)  _sp = 1;
						Sound::PlayTempSound(SND_PLACE_BUILDING, 0.8f, 1.0f);
						BuildTileAt(hovered_tile_x, hovered_tile_y, tileToBuild, _sp);
						tileToBuild = TT_NONE;
					}
				}
				else {
					//demolish
					//temp
					if (LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type != TT_WATER && !(GET_TILE_INFO(LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type).flags & TIF_ALIEN)) {
						//get half the money back
						if ((GET_TILE_INFO(LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type).flags & TIF_HUMAN)) {
							playerCash += GET_TILE_INFO(LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type).buildCost / 2;
						}
						OnTileDestroy(hovered_tile_x, hovered_tile_y, LEVEL.GetTile(hovered_tile_x, hovered_tile_y), true);
					}
				}
			}
		}

		TickCamMovement();
	}

	//clamp camera in screen
	CAMERA_X = std::clamp(CAMERA_X, -16, -SCREEN_W + 16 + LEVEL_W * 8);
	CAMERA_Y = std::clamp(CAMERA_Y, -16, -SCREEN_H + 16 + LEVEL_H * 8);


	//alien mastermind thinks
	if (GAME_TICK % 2 == 0) alienMastermind.Tick();

	//update sound listener
	Sound::SetListenerPos(CAMERA_X + SCREEN_W/2, CAMERA_Y + SCREEN_H/2);
}




void Game::TickCamMovement() {

	int _camSpd = 4;

	if (Input::KeyHeld(SDL_SCANCODE_LSHIFT) || Input::KeyHeld(SDL_SCANCODE_RSHIFT)) {
		_camSpd = 8;
	}

	if (Input::KeyHeld(SDL_SCANCODE_LEFT)) CAMERA_X -= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_RIGHT)) CAMERA_X += _camSpd;

	if (Input::KeyHeld(SDL_SCANCODE_UP)) CAMERA_Y -= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_DOWN)) CAMERA_Y += _camSpd;


	if (draggingCam) {

		cursorState = CS_DRAG;

		int mX = CURSOR_X;
		int mY = CURSOR_Y;


		CAMERA_X = CAMERA_X + camDragX - mX;
		CAMERA_Y = CAMERA_Y + camDragY - mY;

		camDragX = mX;
		camDragY = mY;

		if (!Input::MouseHeld(MB_MIDDLE)) {
			draggingCam = false;
		}
	}
	else {
		if (Input::MouseHeld(MB_MIDDLE)) {
			camDragX = CURSOR_X;
			camDragY = CURSOR_Y;
			draggingCam = true;
		}
	}


	//press space to jump to hq
	//if (Input::KeyPressed(SDL_SCANCODE_SPACE)) {
	//	CAMERA_X = currentLevel.playerHq.origin_x * 8 + 8 - SCREEN_W/2;
	//	CAMERA_Y = currentLevel.playerHq.origin_y * 8 + 8 - SCREEN_H/2;
	//}

	//make sure we're not too far away from our build focus
	const int _maxCamDist_x = 72;//96;
	const int _maxCamDist_y = 64;//80;

	if (playerJet != NULL) {
		CAMERA_X = std::clamp(CAMERA_X, playerJet->x - SCREEN_W/2 - _maxCamDist_x, playerJet->x - SCREEN_W/2 + _maxCamDist_x);
		CAMERA_Y = std::clamp(CAMERA_Y, playerJet->y - SCREEN_H/2 - _maxCamDist_y, playerJet->y - SCREEN_H/2 + _maxCamDist_y);
	} else {
		if (LEVEL.playerHq.flags & CF_ACTIVE) {
			CAMERA_X = std::clamp(CAMERA_X, (LEVEL.playerHq.origin_x * 8) - SCREEN_W / 2 - _maxCamDist_x, (LEVEL.playerHq.origin_x * 8) - SCREEN_W / 2 + _maxCamDist_x);
			CAMERA_Y = std::clamp(CAMERA_Y, (LEVEL.playerHq.origin_y * 8) - SCREEN_H / 2 - _maxCamDist_y, (LEVEL.playerHq.origin_y * 8) - SCREEN_H / 2 + _maxCamDist_y);
		}
	}

}



void Game::Draw() {


	Graphics::DrawRect({0, 0, SCREEN_W, SCREEN_H}, { 192, 231, 247, 255});

	currentLevel.Draw();

	//draw ui at end
	DrawUi();

}



void Game::DrawUi() {

	//news ticker
	if (newsticker_time > -(SCREEN_W + (int)newsticker_text.length() * 8)) {
		Graphics::DrawRect({ _borderW, borderTopSize, SCREEN_W - _borderW * 2, 8 }, { 0, 0, 0, 255 });
		Graphics::DrawText(_borderW + newsticker_time, borderTopSize, newsticker_text, 1);
		if (GAME_TICK % 2 == 0) newsticker_time--;
	}

	//border?
	Graphics::DrawRect({ 0, 0, SCREEN_W, borderTopSize }, { 160, 160, 174, 255 });
	Graphics::DrawRect({ 0, SCREEN_H- borderTopSize, SCREEN_W, borderTopSize }, { 160, 160, 174, 255 });

	Graphics::DrawRect({ 0, 16, _borderW, SCREEN_H-16 }, { 160, 160, 174, 255 });
	Graphics::DrawRect({ SCREEN_W- _borderW, 16, _borderW, SCREEN_H-16 }, { 160, 160, 174, 255 });


	//bearings
	int _camBearingPos = (CAMERA_X >> 3) / 5;
	int _bearingAmt = SCREEN_W / 40;
	int _xx, _yy;
	std::string _txt;

	//top bearings
	for (int i = 1; i < _bearingAmt + 1; i++) {

		_txt = std::to_string((_camBearingPos + i) * 5);

		_xx = (_camBearingPos * 8) * 5 + (i * 40) - CAMERA_X;
		if (_xx < _borderW || _xx > SCREEN_W-16-_borderW) continue;

		if ((_camBearingPos + i) % 2 == 0) {
			//number
			Graphics::DrawText(_xx - _txt.length() * 4 + 1, 9, _txt, 1, { 0, 0, 0 });
			Graphics::DrawText(_xx - _txt.length() * 4, 8, _txt, 1, C_XRED);
		} else {
			//pip
			Graphics::DrawSpr(TEX_CHARS, { _xx - 4, 8, 8, 8 }, { 0, 8, 8, 8 });
		}
	}

	//side bearings
	_camBearingPos = (CAMERA_Y >> 3) / 5;
	_bearingAmt = SCREEN_H / 40;

	//side bearings
	for (int i = 1; i < _bearingAmt + 1; i++) {

		_txt = std::to_string((_camBearingPos + i) * 5);

		_xx = 0;//8;
		//if (_txt.length() >= 3) _xx = 0;

		_yy = -4 + (_camBearingPos * 8) * 5 + (i * 40) - CAMERA_Y;
		if (_yy <= 8) continue;


		if ((_camBearingPos + i) % 2 == 0) {
			//num
			Graphics::DrawText(_xx + 1, _yy + 1, _txt, 1, { 0, 0, 0 });
			Graphics::DrawText(_xx, _yy, _txt, 1, C_YBLUE);
		} else {
			//pip
			Graphics::DrawSpr(TEX_CHARS, { 8, _yy, 8, 8 }, { 0, 16, 8, 8 });
		}


	}

	//x y notifier
	//Graphics::DrawRect({ 0, 8, 24, 8 }, {0,0,0,255});
	//Graphics::DrawText(8, 8, "X", 1, C_XRED);
	//Graphics::DrawText(16, 8, "Y", 1, C_YBLUE);

	Graphics::DrawRect({ 0, 0, _borderW, 16 }, { 160, 160, 174, 255 });
	//Graphics::DrawText(0, 0, "X", 1, C_XRED);
	//Graphics::DrawText(8, 8, "Y", 1, C_YBLUE);

	Graphics::DrawRect({ 0, SCREEN_H - 16, 24, 16 }, { 160, 160, 174, 255 });


	//build UI
	if (state == GS_BUILD) {


		//if hovering over city, draw expanded info
		if (GET_TILE_INFO(LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->type).flags & TIF_CITY) {
			City* _city = &LEVEL.arrCities[LEVEL.GetTile(hovered_tile_x, hovered_tile_y)->owner];

			std::string _txt;
			SDL_Colour _c;
			int _yy = 8 + (_city->origin_y * 8) - CAMERA_Y;

			//expansion info
			if (_city->resources > 0) {
				_txt = "Expanding";
				_c = {0, 205, 0, 255};
			}
			else {
				_txt = "Not Expanding";
				_c = { 205, 0, 0, 255 };
			}

			_xx = (-_txt.length() * 4) + (_city->origin_x * 8) - CAMERA_X;
			Graphics::DrawText(1 + _xx, _yy + 1, _txt, 1, {0, 0, 0, 255});
			Graphics::DrawText(_xx, _yy, _txt, 1, _c);

			//pop info
			_yy += 8;
			_txt = "Population " + std::to_string(_city->popcount);

			_xx = (-_txt.length() * 4) + (_city->origin_x * 8) - CAMERA_X;
			Graphics::DrawText(1 + _xx, _yy + 1, _txt, 1, { 0, 0, 0, 255 });
			Graphics::DrawText(_xx, _yy, _txt, 1, { 0, 205, 0, 255 });

		}


		//draw info about the thing you're gonna try to build
		if (bm_hover != -1 || bm_selected_opt != -1) {

			TileInfo _info;

			if (bm_hover != -1) _info = GET_TILE_INFO(arrBuildOptions[bm_hover]);
			else _info = GET_TILE_INFO(arrBuildOptions[bm_selected_opt]);

			Graphics::DrawRect({ _borderW, SCREEN_H - 32, SCREEN_W - _borderW * 2, 16 }, { 0, 0, 0, 255 });
			//name
			Graphics::DrawText(_borderW, SCREEN_H - 32, _info.name, 1);
			//cost
			Graphics::DrawText(_borderW, SCREEN_H - 24, "$" + std::to_string(_info.buildCost), 1, { 255, 227, 128, 255 });
		}


		int _xx, _yy;
		TileInfo _tileInfo;

		for (int i = 0; i < BUILD_OPTIONS; i++) {
			_xx = bm_startX + i * 24;
			_tileInfo = GET_TILE_INFO(arrBuildOptions[i]);

			if (bm_selected_opt == i) _yy = 0;
			else _yy = 2;

			Graphics::DrawSpr(TEX_CHARS, { _xx, _yy + SCREEN_H-24, 24, 24}, {0, 216, 24, 24});
			

			if (arrBuildOptions[i] == TT_NONE) {
				//demolish
				Graphics::DrawSpr(TEX_CHARS, { _xx + 12 - 8, _yy + SCREEN_H - 12 - 8, 16, 16}, {24, 216, 16, 16});
			} else {
				//tile icon
				Graphics::DrawSpr(TEX_CHARS, { _xx + 12 - _tileInfo.buildSpr.w / 2, _yy + SCREEN_H - 12 - _tileInfo.buildSpr.h / 2, _tileInfo.buildSpr.w, _tileInfo.buildSpr.h }, _tileInfo.buildSpr);
			}

			//selected highlight
			if (bm_hover == i) {
				SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_MUL);
				Graphics::DrawSpr(TEX_CHARS, { _xx, _yy + SCREEN_H - 24, 24, 24 }, { 0, 0, 8, 8 }, {255, 255, 255, 100});
				SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_BLEND);
			}
		}


	}

	//PLAY MODE
	if (state == GS_PLAY) {
		//JET BOMBS
		if (playerJet != NULL && !(playerJet->flags & EFL_DELETED)) {
			Graphics::DrawText(81, SCREEN_H - 15, "MISSILES " + std::to_string(playerMissiles), 1, { 0, 0, 0, 255 });
			Graphics::DrawText(80, SCREEN_H - 16, "MISSILES " + std::to_string(playerMissiles), 1, { 91, 255, 165, 255 });
		}

	}

	//cash counter
	Graphics::DrawText(17, SCREEN_H - 7, "$ " + std::to_string(playerCash), 1, {0, 0, 0, 255});
	Graphics::DrawText(16, SCREEN_H - 8, "$ " + std::to_string(playerCash), 1, {255, 227, 128, 255});

	//JET HP
	if (playerJet != NULL && !(playerJet->flags & EFL_DELETED)) {
		Graphics::DrawText(17, SCREEN_H - 15, "JET " + std::to_string(playerJet->hp), 1, { 0, 0, 0, 255 });
		Graphics::DrawText(16, SCREEN_H - 16, "JET " + std::to_string(playerJet->hp), 1, { 247, 104, 104, 255 });
	}
	//looking at hq
	else if (LEVEL.playerHq.flags & CF_ACTIVE) {
		if (jetBuildTimer <= 0) {
			//jet is ready notice
			Graphics::DrawText(_borderW + 8 + 1, borderTopSize + 8 + 1, "JET READY!", 1, { 0, 0, 0, 255 });
			Graphics::DrawText(_borderW + 8, borderTopSize + 8, "JET READY!", 1, { 255, 227, 128, 255 });
		} else {
			if ((GAME_TICK/3) % 6 < 4) {
				//jet is ready notice
				Graphics::DrawText(_borderW + 8 + 1, borderTopSize + 8 + 1, "PREPARING JET!", 1, { 0, 0, 0, 255 });
				Graphics::DrawText(_borderW + 8, borderTopSize + 8, "PREPARING JET!", 1, { 255, 152, 51, 255 });
			}
		}

		//HQ hp
		//Tile* _t = LEVEL.GetTile(LEVEL.playerHq.origin_x, LEVEL.playerHq.origin_y);
		//Graphics::DrawText(17, SCREEN_H - 15, "HQ " + std::to_string(_t->hp), 1, { 0, 0, 0, 255 });
		//Graphics::DrawText(16, SCREEN_H - 16, "HQ " + std::to_string(_t->hp), 1, { 247, 104, 104, 255 });
	}



	//game over
	if (state == GS_GAMEOVER) {
		SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_MUL);
		Graphics::DrawRect({ 0, 0, SCREEN_W, SCREEN_H }, {255, 0, 0, 100});
		SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_BLEND);

		std::string _str = "GAME OVER";
		Graphics::DrawText(1 + SCREEN_W / 2 - _str.length() * 8, 63, _str, 2, {0, 0, 0, 255});
		Graphics::DrawText(-1 + SCREEN_W / 2 - _str.length() * 8, 65, _str, 2);
	}


	//draw cursor
	switch (cursorState) {
		case CS_POINTER:
			//Graphics::DrawSpr(TEX_CHARS, { CURSOR_X + 2, CURSOR_Y + 2, 8, 8 }, { 0, 248, 8, 8 }, { 0, 0, 0, 255 });
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X, CURSOR_Y, 8, 8 }, { 0, 248, 8, 8 });
		break;
		case CS_DRAG:
			//Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 8 + 2, CURSOR_Y - 8 + 2, 16, 16 }, { 8, 240, 16, 16 }, { 0, 0, 0, 255 });
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 8, CURSOR_Y - 8, 16, 16 }, { 8, 240, 16, 16 });
		break;
		case CS_BUILD_TILE:{
			SDL_Colour _col = {255, 255, 255, (Uint8)(160 + sin(GAME_TICK / 20) * 90) };
			if (!CheckIfCanBuildTile(hovered_tile_x, hovered_tile_y, tileToBuild)) {
				_col.g = 0;
				_col.b = 0;
			} 
			Graphics::DrawSpr(TEX_CHARS, { -CAMERA_X + hovered_tile_x * 8, -CAMERA_Y + hovered_tile_y * 8, GET_TILE_INFO(tileToBuild).buildSpr.w, GET_TILE_INFO(tileToBuild).buildSpr.h }, GET_TILE_INFO(tileToBuild).buildSpr, _col);
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X, CURSOR_Y, 8, 8 }, { 0, 248, 8, 8 });
		break;}
		case CS_CROSSHAIR:
			//Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 4 + 2, CURSOR_Y - 4, 8, 8 }, { 0, 240, 8, 8 }, { 0, 0, 0, 255 });
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 4, CURSOR_Y - 4 - 4, 8, 8 }, { 0, 240, 8, 8 });
		break;

	}



}

void Game::SetMusicTo(int sound) {
	if (mute_music) return;

	if (sound == 0) audio_bgm.Stop();
	else if (audio_bgm.soundBufferID != sound) {
		audio_bgm.Stop();
		audio_bgm.Play(sound, true, false);
	}
}


void Game::AddNews(std::string _txt) {
	newsticker_text = _txt;
	newsticker_time = SCREEN_W;
}
