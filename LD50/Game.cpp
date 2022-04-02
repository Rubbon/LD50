#include "Game.h"
#include "Graphics.h"
#include "Input.h"
#include <string>

bool RUN_GAME = false;
Game GAME = {};




void Game::Init() {
	currentLevel.GenerateWorld();

}

void Game::Tick() {

	int _camSpd = 16;

	if (Input::KeyHeld(SDL_SCANCODE_LEFT)) CAMERA_X-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_RIGHT)) CAMERA_X+= _camSpd;

	if (Input::KeyHeld(SDL_SCANCODE_UP)) CAMERA_Y-= _camSpd;
	else if (Input::KeyHeld(SDL_SCANCODE_DOWN)) CAMERA_Y+= _camSpd;

}


void Game::Draw() {


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

