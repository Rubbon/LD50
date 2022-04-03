#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"

const SDL_Colour C_XRED = { 255, 64, 64 };
const SDL_Colour C_YBLUE = { 104, 170, 255 };


enum CursorState {
	CS_POINTER,
	CS_DRAG,
};

enum GameState {
	GS_PLAY,
};


class Game {
public:

	Level currentLevel = {};
	unsigned char cursorState = CS_POINTER;

	GameState state = GS_PLAY;

	void Init();
	void Tick();
	void Draw();

	void DrawUi();

};



extern bool RUN_GAME;
extern Game GAME;
