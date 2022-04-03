#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include <string>
#include <algorithm>

bool RUN_GAME = false;
Game GAME = {};




void Game::Init() {
	
	LevelGenerator _gen = {};

	_gen.GenerateWorld(&currentLevel);

}



//temp (for screen drag)
bool draggingCam = false;
int camDragX, camDragY;


void Game::Tick() {
	//reset cursor state first thing
	cursorState = CS_POINTER;

	int _camSpd = 8;

	if (Input::KeyHeld(SDL_SCANCODE_LEFT)) CAMERA_X-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_RIGHT)) CAMERA_X+= _camSpd;

	if (Input::KeyHeld(SDL_SCANCODE_UP)) CAMERA_Y-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_DOWN)) CAMERA_Y+= _camSpd;


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


}


void Game::Draw() {


	Graphics::DrawRect({0, 0, SCREEN_W, SCREEN_H}, { 192, 231, 247, 255});

	//draw current world tiles
	int _ctx = CAMERA_X >> 3;
	int _cty = CAMERA_Y >> 3;
	int ix, iy;

	//draw tiles
	for (ix = _ctx; ix <= 1 + _ctx + Graphics::SCREEN_W / 8; ix++) {
		if (ix < 0 || ix >= LEVEL_W) continue;
		for (iy = _cty; iy <= 1 + _cty + Graphics::SCREEN_H / 8; iy++) {
			if (iy < 0 || iy >= LEVEL_H) continue;
			TileDraw((ix * 8) - CAMERA_X, (iy * 8) - CAMERA_Y, ix, iy, currentLevel.GetTile(ix, iy));
		}
	}


	//cities draw their name TODO - DON'T DRAW WHEN OUTSIDE SCREEN
	for (int i = 0; i < MAX_CITIES; i++) {

		int _xx = ( - currentLevel.arrCities[i].name.length() * 4) + (currentLevel.arrCities[i].origin_x * 8) - CAMERA_X;

		Graphics::DrawText(1 + _xx, 1 +(currentLevel.arrCities[i].origin_y * 8) - CAMERA_Y, currentLevel.arrCities[i].name, 1, {0, 0, 0});
		Graphics::DrawText(_xx, (currentLevel.arrCities[i].origin_y * 8) - CAMERA_Y, currentLevel.arrCities[i].name, 1, {255, 227, 128});
	}






	//draw ui at end
	DrawUi();

}




void Game::DrawUi() {

	//bearings
	int _camBearingX = (CAMERA_X >> 3) / 10;
	int _xBearingAmt = SCREEN_W / 80;


	for (int i = 1; i < _xBearingAmt+1; i++) {
		Graphics::DrawText(1 + (_camBearingX * 8) * 10 + (i * 80) - CAMERA_X, 9, std::to_string((_camBearingX + i) * 10), 1, {0, 0, 0});
		Graphics::DrawText((_camBearingX * 8) * 10 + (i * 80) - CAMERA_X, 8, std::to_string((_camBearingX + i) * 10), 1, {255, 64, 64});
	}


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
	}

}

