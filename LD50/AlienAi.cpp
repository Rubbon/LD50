#include "AlienAi.h"
#include <random>
#include "Game.h"
#include <iostream>
#include "Input.h"
#include "EnemyEntities.h"

int reconTick = 32;
int attackTick = 256;
int warTick = 2000;

int lastAttackedHq = 0;

void AlienMastermind::Tick(){

	TryDoingRecon();
	TryDoingAttack();


	if (warTick <= 0) {
		//war +
		std::cout << "war +1" << std::endl;
		if (warStage < 16) warStage++;

		warTick = 2000 + warStage * 24;
	}

	//update parties
	if (GAME_TICK % 3 == 0) {
		for (int i = 0; i < vAttackParties.size(); i++) {
			//remove if everyones gone
			if (vAttackParties[i].vEntities.size() <= 0) {
				vAttackParties.erase(vAttackParties.begin() + i);
				continue;
			}

			//remove dead aliens
			for (int e = 0; e < vAttackParties[i].vEntities.size(); e++) {
				if (vAttackParties[i].vEntities[e]->flags & EFL_DELETED) {
					vAttackParties[i].vEntities.erase(vAttackParties[i].vEntities.begin() + e);
				}
			}

			//update for flanking?

		}

		for (int i = 0; i < vActiveAlienUnits.size(); i++) {
			//remove if dead
			if (vActiveAlienUnits[i]->flags & EFL_DELETED) {
				vActiveAlienUnits.erase(vActiveAlienUnits.begin() + i);
			}
		}

	}


}




void AlienMastermind::TryDoingRecon() {
	Entity* _ent;

	reconTick--;

	//rcon
	if (reconTick <= 0) {

		//check if we need to make a new recon unit
		if (_reconUnit == NULL || (_reconUnit->flags & EFL_DELETED) || _reconUnit->state != ES_RECON) {
			//make one for now
			_ent = LEVEL.AddEntity((rand() % 2) * LEVEL_W*8, 8 + rand() % (LEVEL_H/2) * 8, ENT_UFO);
			//vActiveAlienUnits.push_back(_ent);
			_reconUnit = _ent;
			_reconUnit->state = ES_RECON;
			_reconUnit->substate = 0;
			_reconUnit->wait = 0;
			_reconUnit->ticker = 0;
		}
		else {
			//if recon unit isn't done, wait a bit
			if (_reconUnit->substate != 2) {
				reconTick += 256;
				return;
			}
		}

		//find a cool pos to recon
		searchLocation.x = 1 + rand() % ((LEVEL_W / RECON_SPACE_SIZE) - 2);
		searchLocation.y = 1 + rand() % ((LEVEL_H / RECON_SPACE_SIZE) - 2);

		//cheating, for setting the pos
		//searchLocation.x = (GAME.hovered_tile_x / RECON_SPACE_SIZE);
		//searchLocation.y = (GAME.hovered_tile_y / RECON_SPACE_SIZE);

		//if its not the same place as last time, go and look there
		if (searchLocation.x != lastSearchLocation.x || searchLocation.y != lastSearchLocation.y) {
			_reconUnit->target_x = (searchLocation.x * RECON_SPACE_SIZE) * 8;
			_reconUnit->target_y = (searchLocation.y * RECON_SPACE_SIZE) * 8;
			_reconUnit->substate = 0;
			_reconUnit->wait = 0;
			_reconUnit->ticker = 0;

			std::cout << "RECON CHECKING OUT " << searchLocation.x * RECON_SPACE_SIZE << ", " << searchLocation.y * RECON_SPACE_SIZE << std::endl;

		}

		reconTick = 512 - warStage * 2;

	}
}





void AlienMastermind::TryDoingAttack() {

	Entity* _ent;

	//attack something from outside world
	if (attackTick <= 0) {

		if (vAttackParties.size() < warStage + 1) {

			int _targetX = -1;
			int _targetY = -1;
			int _r;

			//find something to attack

			//areas of interest first
			if (vAreasOfInterest.size() > 0) {

				_r = rand() % vAreasOfInterest.size();

				_targetX = vAreasOfInterest[_r].x;
				_targetY = vAreasOfInterest[_r].y;

				//remove once attacked
				vAreasOfInterest.erase(vAreasOfInterest.begin() + _r);

				goto lEndOfLocationSearch;
			}

			//then hq
			if (foundHqPos.x != -1 && GAME_TICK - lastAttackedHq > 1024 - warStage * 2 + rand() % 256) {
				_targetX = foundHqPos.x;
				_targetY = foundHqPos.y;
				lastAttackedHq = GAME_TICK;
				goto lEndOfLocationSearch;
			}

			//then city
			_r = rand() % MAX_CITIES;

			if ((LEVEL.arrCities[_r].flags & CF_FOUND) && (LEVEL.arrCities[_r].flags & CF_ACTIVE)) {
				_targetX = LEVEL.arrCities[_r].origin_x + -2 + rand() % 6;
				_targetY = LEVEL.arrCities[_r].origin_y + -2 + rand() % 6;
				goto lEndOfLocationSearch;
			}


			//consider empty pltos
			if (vEmptyPlots.size() > 0 && rand() % (8 + warStage / 2) >= 8) {

				_r = rand() % vEmptyPlots.size();

				_targetX = vEmptyPlots[_r].x;
				_targetY = vEmptyPlots[_r].y;

				//remove once attacked
				vEmptyPlots.erase(vEmptyPlots.begin() + _r);
			}



			lEndOfLocationSearch:

			if (_targetX == -1) return;

			std::cout << "Sending out new attack party to " << _targetX	<< ", " << _targetY << std::endl;

			//make new attack party
			AlienParty _party = {};

			int _yy = rand()%LEVEL_H/2;
			int	_xx = (rand() % 2) * LEVEL_W * 8;

			for (int i = 0; i < 6 + warStage * 2 + (rand() % warStage * 3); i++) {
				_ent = LEVEL.AddEntity(_xx, (LEVEL_H/4) + _yy + (rand()%64) - 32, ENT_UFO);
				_ent->state = ES_ATTACKER;
				_party.vEntities.push_back(_ent);
			}

			//cance of spawning walkers
			if (warStage >= 1) {
				_xx = (rand() % 2) * LEVEL_W * 8;
				for (int i = 0; i < 2 + warStage * 2 + (rand() % warStage * 4); i++) {
					_ent = LEVEL.AddEntity(_xx, (LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_WALKER);
					_party.vEntities.push_back(_ent);
				}
			}

			//add a recon
			_ent = LEVEL.AddEntity(0, 0, ENT_UFO);
			_ent->state = ES_RECON;
			_party.vEntities.push_back(_ent);

			_party.gather_x = _targetX;
			_party.gather_y = _targetY;

			_party.TellEntitiesToGather();

			vAttackParties.push_back(_party);

			attackTick = 1024 + (rand() % 256) - warStage * 3;
		}


	}


	//consider adding a big ufo
	if (warStage >= 1) {
		int	_xx = (rand() % 2) * LEVEL_W * 8;
		if (rand() % 8 == 0) {
			if (vActiveAlienUnits.size() < warStage + 1) {
				int _yy = rand() % LEVEL_H / 2;
				_ent = LEVEL.AddEntity(_xx, (LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_E_ALIEN_HUNTER);
				vActiveAlienUnits.push_back(_ent);
			}
		}
	}


	//consider spawning some walkers
	if (rand() % 3 == 0) {
		if (vActiveAlienUnits.size() < (warStage + 1) * 5) {
			int _xx = (rand() % 2) * LEVEL_W * 8;
			int _yy = rand() % LEVEL_H / 2;
			for (int i = 0; i < 2 + warStage * 2 + (rand() % warStage * 4); i++) {
				_ent = LEVEL.AddEntity(_xx, (LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_WALKER);
				vActiveAlienUnits.push_back(_ent);
			}
		}
	}

	//consider spawning some walkers somewhere random in water
	for (int i = 0; i < warStage; i++) {
		int _xx = rand() % LEVEL_W;
		int _yy = rand() % LEVEL_H;

		if (LEVEL.GetTile(_xx, _yy)->type == TT_WALL) {
			LEVEL.AddEntity(_xx * 8, _yy * 8, ENT_WALKER);
		}
	}
	

	attackTick--;

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
