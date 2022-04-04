#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"
#include "AlienAi.h"

const SDL_Colour C_XRED = { 255, 64, 64 };
const SDL_Colour C_YBLUE = { 32, 32, 255 };


enum CursorState {
	CS_POINTER,
	CS_DRAG,
	CS_NOCURSOR,
	CS_BUILD_TILE,
	CS_CROSSHAIR,
};

enum GameState {
	GS_BUILD_HQ,
	GS_PLAY,
	GS_BUILD,
};


class Game {
public:

	Level currentLevel = {};
	unsigned char cursorState = CS_POINTER;

	GameState state = GS_PLAY;

	//building tiles
	TileType tileToBuild = TT_NONE;

	//alien ai controller
	AlienMastermind alienMastermind = {};


	Entity* playerJet = NULL;


	int hovered_tile_x = 0;
	int hovered_tile_y = 0;

	int gameTick = 0;

	void Init();
	void Tick();
	void Draw();

	void TickCamMovement();

	void DrawUi();

};



extern bool RUN_GAME;
extern Game GAME;

#define GAME_TICK GAME.gameTick
#define LEVEL GAME.currentLevel