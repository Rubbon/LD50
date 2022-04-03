#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"

class AlienMastermind {
public:
	std::vector<Entity*> vActiveAlienUnits;

	short warStage = 0;



	//recon
	Pos searchLocation = {0,0};
	Pos lastSearchLocation = {0,0};
	Entity* _reconUnit = NULL;


	void Tick();

};