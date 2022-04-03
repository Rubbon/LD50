#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"


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
