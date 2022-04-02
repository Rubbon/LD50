#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"


enum CursorState {
	CS_POINTER,
	CS_DRAG,
};



class Game {
public:

	Level currentLevel = {};
	unsigned char cursorState = CS_POINTER;

	void Init();
	void Tick();
	void Draw();


};



extern bool RUN_GAME;
extern Game GAME;
