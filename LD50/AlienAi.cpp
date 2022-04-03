#include "AlienAi.h"
#include <random>
#include "Game.h"
#include <iostream>
#include "Input.h"
#include "EnemyEntities.h"

int reconTick = 128;

void AlienMastermind::Tick(){

	Entity* _ent;

	TryDoingRecon();


	//attack something from outside world
	if (Input::KeyPressed(SDL_SCANCODE_K)) {

		std::cout << "KILL" << std::endl;

		AlienParty _party = {};

		for (int i = 0; i < 6; i++) {
			_ent = LEVEL.AddEntity(0, 0, ENT_UFO);
			_ent->state = ES_ATTACKER;
			_party.vEntities.push_back(_ent);
		}

		_party.gather_x = GAME.hovered_tile_x;
		_party.gather_y = GAME.hovered_tile_y;

		_party.TellEntitiesToGather();

		vAttackParties.push_back(_party);

	}



}




void AlienMastermind::TryDoingRecon() {
	Entity* _ent;

	//rcon
	if (reconTick <= 0) {

		//check if we need to make a new recon unit
		if (_reconUnit == NULL || (_reconUnit->flags & EFL_DELETED) || _reconUnit->state != ES_RECON) {
			//make one for now
			_ent = LEVEL.AddEntity((rand() % 2) * LEVEL_W*8, 2 + rand() % (LEVEL_H - 4) * 8, ENT_UFO);
			//vActiveAlienUnits.push_back(_ent);
			_reconUnit = _ent;
			_reconUnit->state = ES_RECON;
			_reconUnit->substate = 0;
			_reconUnit->wait = 0;
		}
		else {
			//if recon unit isn't done, wait a bit
			if (_reconUnit->substate != 2) reconTick += 256;
			return;
		}

		//find a cool pos to recon
		searchLocation.x = 1 + rand() % (LEVEL_W / RECON_SPACE_SIZE) - 2;
		searchLocation.y = 1 + rand() % (LEVEL_H / RECON_SPACE_SIZE) - 2;

		//cheating, for setting the pos
		//searchLocation.x = (GAME.hovered_tile_x / RECON_SPACE_SIZE);
		//searchLocation.y = (GAME.hovered_tile_y / RECON_SPACE_SIZE);

		//if its not the same place as last time, go and look there
		if (searchLocation.x != lastSearchLocation.x || searchLocation.y != lastSearchLocation.y) {
			_reconUnit->target_x = (searchLocation.x * RECON_SPACE_SIZE) * 8;
			_reconUnit->target_y = (searchLocation.y * RECON_SPACE_SIZE) * 8;

			std::cout << "RECON CHECKING OUT " << searchLocation.x * RECON_SPACE_SIZE << ", " << searchLocation.y * RECON_SPACE_SIZE << std::endl;

		}

		reconTick = 512 - warStage * 2;

	}
	else {
		reconTick--;
	}
}




void AlienParty::TellEntitiesToGather() {
	for (int i = 0; i < vEntities.size(); i++) {

		if (vEntities[i]->flags & EFL_DELETED) {
			vEntities.erase(vEntities.begin() + i);
			//i--;
			continue;
		}

		vEntities[i]->target_x = (gather_x + sin(i + (rand() % 3) - 1) * vEntities.size()/2) * 8;
		vEntities[i]->target_y = (gather_y + cos(i + (rand() % 3) - 1) * vEntities.size()/2) * 8;
	}
}
