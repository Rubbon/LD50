#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"



class Game {
public:

	Level currentLevel = {};


	void Init();
	void Tick();
	void Draw();


};



extern bool RUN_GAME;
extern Game GAME;
