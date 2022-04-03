#include "AlienAi.h"
#include <random>
#include "Game.h"
#include <iostream>
#include "Input.h"
#include "EnemyEntities.h"

void AlienMastermind::Tick(){

	Entity* _ent;

	//test recon
	if (Input::KeyPressed(SDL_SCANCODE_R)) {
		std::cout << "RECON TICK" << std::endl;

		//check if we already have a recon unit

		//make one for now
		_ent = LEVEL.AddEntity(0, 0, ENT_UFO);
		vActiveAlienUnits.push_back(_ent);
		_reconUnit = _ent;
		_reconUnit->state = ES_RECON;

		//find a cool pos to recon
		searchLocation.x = rand() % (LEVEL_W / RECON_SPACE_SIZE);
		searchLocation.y = rand() % (LEVEL_H / RECON_SPACE_SIZE);
		
		//if its not the same place as last time, go and look there
		if (searchLocation.x != lastSearchLocation.x || searchLocation.y != lastSearchLocation.y) {
			_reconUnit->target_x = (searchLocation.x * RECON_SPACE_SIZE) * 8;
			_reconUnit->target_y = (searchLocation.y * RECON_SPACE_SIZE) * 8;

			std::cout << "RECON CHECKING OUT " << searchLocation.x * RECON_SPACE_SIZE << ", " << searchLocation.y * RECON_SPACE_SIZE << std::endl;

		}

	}


}
