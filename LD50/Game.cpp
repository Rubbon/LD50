#include "Game.h"
#include "Graphics.h"
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
}



//temp (for screen drag)
bool draggingCam = false;
int camDragX, camDragY;


void Game::Tick() {
	//reset cursor state first thing
	cursorState = CS_POINTER;

	gameTick++;

	currentLevel.Tick();


	
	hovered_tile_x = (CURSOR_X + CAMERA_X) >> 3;
	hovered_tile_y = (CURSOR_Y + CAMERA_Y) >> 3;

	//building tile
	if (tileToBuild != TT_NONE) {
		cursorState = CS_BUILD_TILE;


		if (Input::MousePressed(MB_LEFT)) {
			if (CheckIfCanBuildTile(hovered_tile_x, hovered_tile_y, tileToBuild)) {
				BuildTileAt(hovered_tile_x, hovered_tile_y, tileToBuild);
				tileToBuild = TT_NONE;
			}
		}

	}


	TickCamMovement();

	//alien mastermind thinks
	if (GAME_TICK % 2 == 0) alienMastermind.Tick();


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

	//clamp camera in screen
	CAMERA_X = std::clamp(CAMERA_X, 0, -SCREEN_W + LEVEL_W * 8);
	CAMERA_Y = std::clamp(CAMERA_Y, 0, -SCREEN_H + LEVEL_H * 8);


	//press space to jump to hq
	if (Input::KeyPressed(SDL_SCANCODE_SPACE)) {
		CAMERA_X = currentLevel.playerHq.origin_x * 8 + 8 - SCREEN_W/2;
		CAMERA_Y = currentLevel.playerHq.origin_y * 8 + 8 - SCREEN_H/2;
	}

}



void Game::Draw() {


	Graphics::DrawRect({0, 0, SCREEN_W, SCREEN_H}, { 192, 231, 247, 255});

	currentLevel.Draw();


	//draw ui at end
	DrawUi();

}



void Game::DrawUi() {

	//bearings
	int _camBearingPos = (CAMERA_X >> 3) / 10;
	int _bearingAmt = SCREEN_W / 80;
	int _xx, _yy;
	std::string _txt;

	//top bearings
	for (int i = 1; i < _bearingAmt +1; i++) {

		_txt = std::to_string((_camBearingPos + i) * 10);

		_xx = -_txt.length() * 4 + (_camBearingPos * 8) * 10 + (i * 80) - CAMERA_X;
		if (_xx <= 8) continue;

		Graphics::DrawText(_xx + 1, 9, _txt, 1, {0, 0, 0});
		Graphics::DrawText(_xx, 8, _txt, 1, C_XRED);
	}

	//side bearings
	_camBearingPos = (CAMERA_Y >> 3) / 10;
	_bearingAmt = SCREEN_H / 80;

	//side bearings
	for (int i = 1; i < _bearingAmt + 1; i++) {

		_txt = std::to_string((_camBearingPos + i) * 10);

		_xx = 8;
		if (_txt.length() >= 3) _xx = 0;

		_yy = -4 + (_camBearingPos * 8) * 10 + (i * 80) - CAMERA_Y;
		if (_yy <= 8) continue;

		Graphics::DrawText(_xx + 1, _yy + 1, _txt, 1, { 0, 0, 0 });
		Graphics::DrawText(_xx, _yy, _txt, 1, C_YBLUE);
	}

	//x y notifier
	Graphics::DrawRect({ 0, 8, 24, 8 }, {0,0,0,255});
	Graphics::DrawText(8, 8, "X", 1, C_XRED);
	Graphics::DrawText(16, 8, "Y", 1, C_YBLUE);



	//draw cursor
	switch (cursorState) {
		case CS_POINTER:
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X + 2, CURSOR_Y + 2, 8, 8 }, { 0, 248, 8, 8 }, { 0, 0, 0, 255 });
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X, CURSOR_Y, 8, 8 }, { 0, 248, 8, 8 });
		break;
		case CS_DRAG:
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 8 + 2, CURSOR_Y - 8 + 2, 16, 16 }, { 8, 240, 16, 16 }, { 0, 0, 0, 255 });
			Graphics::DrawSpr(TEX_CHARS, { CURSOR_X - 8, CURSOR_Y - 8, 16, 16 }, { 8, 240, 16, 16 });
		break;
		case CS_BUILD_TILE:
			SDL_Colour _col = {255, 255, 255, (Uint8)(160 + sin(GAME_TICK / 20) * 90) };
			if (!CheckIfCanBuildTile(hovered_tile_x, hovered_tile_y, tileToBuild)) {
				_col.g = 0;
				_col.b = 0;
			} 
			Graphics::DrawSpr(TEX_CHARS, { -CAMERA_X + hovered_tile_x * 8, -CAMERA_Y + hovered_tile_y * 8, GET_TILE_INFO(tileToBuild).buildSpr.w, GET_TILE_INFO(tileToBuild).buildSpr.h }, GET_TILE_INFO(tileToBuild).buildSpr, _col);
		break;
	}

}

