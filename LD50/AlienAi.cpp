#include "AlienAi.h"
#include <random>
#include "Game.h"
#include <iostream>
#include "Input.h"

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


	}


}
