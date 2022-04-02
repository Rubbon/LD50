#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include <string>

bool RUN_GAME = false;
Game GAME = {};




void Game::Init() {
	currentLevel.GenerateWorld();

	currentLevel.GetTile(0, 0)->type = TT_LAND;
	currentLevel.GetTile(LEVEL_W-1, LEVEL_H-1)->type = TT_LAND;


}

void Game::Tick() {

	int _camSpd = 8;

	if (Input::KeyHeld(SDL_SCANCODE_LEFT)) CAMERA_X-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_RIGHT)) CAMERA_X+= _camSpd;

	if (Input::KeyHeld(SDL_SCANCODE_UP)) CAMERA_Y-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_DOWN)) CAMERA_Y+= _camSpd;

	if (CAMERA_X < 0) CAMERA_X = 0;
	else if (CAMERA_X+SCREEN_W > LEVEL_W * 8) CAMERA_X = -SCREEN_W + LEVEL_W * 8;

	if (CAMERA_Y < 0) CAMERA_Y = 0;
	else if (CAMERA_Y+SCREEN_H > LEVEL_H * 8) CAMERA_Y = -SCREEN_H + LEVEL_H * 8;

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

}

