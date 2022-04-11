#include "AlienAi.h"
#include <random>
#include "Game.h"
#include <iostream>
#include "Input.h"
#include "EnemyEntities.h"
#include "TileBaseInfo.h"

int reconTick = 32;
int attackTick = 256;
int superAttackTick = 1400;
int warTick = 1300;
int hiveTick = 80;

int attackPartySize = 20;

int lastAttackedHq = 0;


int superAttack_city = 0;


void AlienMastermind::Tick(){


	TryDoingRecon();

	TryDoingAttack();



	//update hives
	if (hiveTick <= 0) {

		//see if we can make a new hive
		int _r = rand() % MAX_CITIES;
		
		if (!(LEVEL.arrCities[_r].flags & CF_ACTIVE) && LEVEL.arrCities[_r].flags & CF_FOUND) {
			if (LEVEL.arrCities[_r].origin_x != -1) {

				//std::cout << "new hive" << std::endl;
				
				//add our alien city
				AlienHive _hive = {};
				_hive.index = _r;
				_hive.origin_x = LEVEL.arrCities[_r].origin_x;
				_hive.origin_y = LEVEL.arrCities[_r].origin_y;
				_hive.Grow();
				vHives.push_back(_hive);

				//make it so we can't use this city again
				LEVEL.arrCities[_r].origin_x = -1;
				LEVEL.arrCities[_r].origin_y = -1;
			}
		}


		if (vHives.size() > 0) {
			for (int i = 0; i < vHives.size(); i++) {
				//remove dead hives
				if (vHives[i].vTiles.size() <= 0) {
					vHives.erase(vHives.begin() + i);
					continue;
				}
				vHives[i].Grow();
			}
		}

		hiveTick = 80 + rand() % 64;
	} else {
		hiveTick--;
	}


	if (warTick <= 0) {
		//war +
		if (warStage < 14) warStage++;

		warTick = 2400 + warStage * 12;
	} else {
		warTick--;
	}


	if (superAttackTick <= 0) {
		//std::cout << "superattack" << std::endl;

		if (superAttack_city != -1) {
			//tell all units to attack this spot
			for (int i = 0; i < vAttackParties.size(); i++) {
				vAttackParties[i].gather_x = LEVEL.arrCities[superAttack_city].origin_x;
				vAttackParties[i].gather_y = LEVEL.arrCities[superAttack_city].origin_y;
			}
			/*
			for (int i = 0; i < vActiveAlienUnits.size(); i++) {
				vActiveAlienUnits[i]->target_x = LEVEL.arrCities[superAttack_city].origin_x;
				vActiveAlienUnits[i]->target_y = LEVEL.arrCities[superAttack_city].origin_y;
			}
			*/

		}

		superAttackTick = 1600 - warStage * 24;
	}
	else {

		if (superAttackTick == 60) {
			//choose a city to attack
			superAttack_city = -1;

			for (int i = 0; i < 6 + warStage; i++){
				int _r = rand() % MAX_CITIES;
				if ((LEVEL.arrCities[_r].flags & CF_FOUND) && (LEVEL.arrCities[_r].flags & CF_ACTIVE)) {
					superAttack_city = _r;
					break;
				}
			}

			//warn if player has scanner
			if (superAttack_city != -1) {
				if (GAME.numberOfScanners > 0) GAME.AddNews("The aliens are gathering for a big attack on " + LEVEL.arrCities[superAttack_city].name + " at " + std::to_string(LEVEL.arrCities[superAttack_city].origin_x) + ", " + std::to_string(LEVEL.arrCities[superAttack_city].origin_y));
			}

			//spawn attack party early
			attackTick = 30;

		}

		superAttackTick--;
	}


	//update parties
	if (GAME_TICK % 3 == 0) {

		/*
		if (vAttackParties.size() > 0) {
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
				//if (vAttackParties[i].start_time )

			}
		}
		*/

		if (vActiveAlienUnits.size() > 0) {
			for (int i = 0; i < vActiveAlienUnits.size(); i++) {
				//remove if dead
				if (vActiveAlienUnits[i]->flags & EFL_DELETED) {
					vActiveAlienUnits.erase(vActiveAlienUnits.begin() + i);
					continue;
				}
			}
		}

	}
	
}




void AlienMastermind::TryDoingRecon() {
	Entity* _ent;

	reconTick--;

	//rcon
	if (reconTick <= 0) {

		Entity* _reconUnit;

		//remove dead recons
		if (vReconUnits.size() > 0) {
			for (int i = 0; i < vReconUnits.size(); i++) {
				if ((vReconUnits[i]->flags & EFL_DELETED) || vReconUnits[i]->state != ES_RECON) {
					vReconUnits.erase(vReconUnits.begin() + i);
					continue;
				}
			}
		}

		//check if we need to make a new recon unit
		//if (_reconUnit == NULL || (_reconUnit->flags & EFL_DELETED) || _reconUnit->state != ES_RECON) {
		if (vReconUnits.size() < 1 + (warStage/1.5)) {
			//make one for now
			_reconUnit = LEVEL.AddEntity((rand() % 2) * LEVEL_W*8, 8 + rand() % (LEVEL_H/2) * 8, ENT_UFO);
			//vActiveAlienUnits.push_back(_ent);
			//_reconUnit = _ent;
			_reconUnit->state = ES_RECON;
			_reconUnit->substate = 3;
			_reconUnit->wait = 0;
			_reconUnit->ticker = 0;
			vReconUnits.push_back(_reconUnit);
		} else {
			//if recon unit isn't done, wait a bit
			//if (_reconUnit->substate != 2) {
			//	reconTick += 256;
			//	return;
			//}

			for (int i = 0; i < vReconUnits.size(); i++) {
				if (vReconUnits[i]->substate != 2) {
					reconTick += 256;
					return;
				}
			}

		}

		for (int i = 0; i < vReconUnits.size(); i++) {

			_reconUnit = vReconUnits[i];

			if (_reconUnit->substate < 1) continue;

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
				lastSearchLocation.x = searchLocation.x;
				lastSearchLocation.y = searchLocation.y;

				//std::cout << "RECON #" << std::to_string(i) << " CHECKING OUT " << searchLocation.x * RECON_SPACE_SIZE << ", " << searchLocation.y * RECON_SPACE_SIZE << std::endl;

			}

		}

		reconTick = 512 - warStage * 2;

	}
}





void AlienMastermind::TryDoingAttack() {

	Entity* _ent;

	if (attackTick % 32 == 0) {

		//consider adding a big ufo
		if (warStage >= 2) {
			if (rand() % 7 == 0) {
				int	_xx = (rand() % 2) * LEVEL_W * 8;
				if (rand() % 8 == 0) {
					if (vActiveAlienUnits.size() < warStage*1.8) {
						int _yy = 8 * (rand() % LEVEL_H / 2);
						_ent = LEVEL.AddEntity(_xx, (8 * LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_E_ALIEN_HUNTER);
						vActiveAlienUnits.push_back(_ent);
						//std::cout << "random big ufo " << std::endl;
					}
				}
			}

			//consider spawning some walkers
			if (rand() % 12 == 0) {
				if (vActiveAlienUnits.size() < (warStage + 1)) {
					int _xx = (rand() % 2) * LEVEL_W * 8;
					int _yy = 8 * (rand() % LEVEL_H / 2);
					for (int i = 0; i < 2 + warStage * 2 + (rand() % warStage * 4); i++) {
						_ent = LEVEL.AddEntity(_xx, (8 * LEVEL_H / 4) + _yy + ((rand() % 64) - 32) * 8, ENT_WALKER);
						_ent->target_x = (24 + (rand() % (LEVEL_W - 48))) * 8;
						_ent->target_y = (24 + (rand() % (LEVEL_H - 48))) * 8;
						vActiveAlienUnits.push_back(_ent);
					}
					//std::cout << "random walker pack " << std::endl;
				}
			}

		}


		//consider spawning some walkers somewhere random in water
		if ((rand() % 5 == 0) && vActiveAlienUnits.size() < warStage*1.6) {
			for (int i = 0; i < warStage / 2; i++) {
				int _xx = 24 + (rand() % (LEVEL_W - 48));
				int _yy = 24 + (rand() % (LEVEL_H - 48));

				if (LEVEL.GetTile(_xx, _yy)->type == TT_WATER) {
					_ent = LEVEL.AddEntity(_xx * 8, _yy * 8, ENT_WALKER);
					vActiveAlienUnits.push_back(_ent);
					//std::cout << "random walker " << std::endl;
				}
			}
		}

		if (_helpessMan == NULL || (_helpessMan->flags & EFL_DELETED) || _helpessMan->entityIndex != ENT_HELPLESSMAN) {
			int _xx = 24 + (rand() % (LEVEL_W - 48));
			int _yy = 24 + (rand() % (LEVEL_H - 48));

			if (GET_TILE_INFO(LEVEL.GetTile(_xx, _yy)->type).flags & TIF_WALKABLE) {
				_helpessMan = LEVEL.AddEntity(_xx * 8, _yy * 8, ENT_HELPLESSMAN);
			}
		}

	}

	attackTick--;

	//attack something from outside world
	if (attackTick <= 0) {

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

			//std::cout << "Sending out new attack party to " << _targetX	<< ", " << _targetY << std::endl;

			if (vAttackParties.size() < warStage + 1) {

				//make new attack party
				AlienParty _party = {};

				int _yy = 8*(rand()%LEVEL_H/2);
				int	_xx = (rand() % 2) * LEVEL_W * 8;

				//fluctuate attack parties
				if (attackPartySize >= 2 + warStage) {
					attackPartySize = 1 + rand() % 2;
				} else {
					attackPartySize = 1 + rand() % (2 + warStage);
				}

				//std::cout << "add ufos" << std::endl;
				for (int i = 0; i < attackPartySize + warStage * 2 + (rand() % warStage * 3); i++) {
					_ent = LEVEL.AddEntity(_xx + (rand() % 16) - 8, 8 * (LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_UFO);
					_ent->state = ES_ATTACKER;
					_party.vEntities.push_back(_ent);
				}
				//std::cout << "add walkers" << std::endl;
				//cance of spawning walkers
				if (warStage >= 2 && rand() % 3 == 0) {
					_xx = (rand() % 2) * LEVEL_W * 8;
					for (int i = 0; i < 1 + (warStage / 2) + (rand() % warStage * 2); i++) {
						_ent = LEVEL.AddEntity(_xx, 8*(LEVEL_H / 4) + _yy + (rand() % 64) - 32, ENT_WALKER);
						_party.vEntities.push_back(_ent);
					}
				}

				//add a recon
				_ent = LEVEL.AddEntity(0, 0, ENT_UFO);
				_ent->state = ES_RECON;
				_party.vEntities.push_back(_ent);

				_party.gather_x = _targetX;
				_party.gather_y = _targetY;
				_party.start_time = GAME_TICK;

				_party.TellEntitiesToGather();

				vAttackParties.push_back(_party);

				attackTick = 1800 + (rand() % 256) - warStage * 3;

			}
			else {
				//tell an old attack party to go somewhere else maybe
				for (int i = 0; i < vAttackParties.size(); i++) {

					//remove dead aliens
					for (int e = 0; e < vAttackParties[i].vEntities.size(); e++) {
						if (vAttackParties[i].vEntities[e]->flags & EFL_DELETED) {
							vAttackParties[i].vEntities.erase(vAttackParties[i].vEntities.begin() + e);
						}
					}

					//remove if everyones gone
					if (vAttackParties[i].vEntities.size() <= 0) {
						vAttackParties.erase(vAttackParties.begin() + i);
						continue;
					}

					if (GAME_TICK - vAttackParties[i].start_time > 2000) {
						vAttackParties[i].gather_x = _targetX;
						vAttackParties[i].gather_x = _targetY;
						vAttackParties[i].start_time = GAME_TICK;
						vAttackParties[i].TellEntitiesToGather();
					}
				}
			}


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




void AlienHive::Grow() {

	if (growtick <= 0) {

		Tile* _t;

		int _placeX = origin_x;
		int _placeY = origin_y;

		int _growthAmt = 1 + vTiles.size() * 2;

		for (int i = 0; i < _growthAmt; i++) {

			_t = LEVEL.GetTile(_placeX, _placeY);

			if (_t->type == TT_LAND || _t->type == TT_CRATER) {

				//type
				TileType _tt = TT_ALIEN_HIVE;
				if (rand() % 6 == 0) _tt = TT_ALIEN_AA_GUN;

				_t->type = _tt;
				_t->owner = index;
				_t->hp = GET_TILE_INFO(_tt).baseHp;
				if (_tt == TT_ALIEN_AA_GUN) LEVEL.vTilesToTick.push_back({ (short)_placeX, (short)_placeY });
				vTiles.push_back({ (short)_placeX, (short)_placeY });

				//stop
				break;
			} else {
				//move
				_placeX += (rand() % 3) - 1;
				_placeY += (rand() % 3) - 1;
				if (_placeX == 0 && _placeY == 0) {
					_placeX = -1;
					_placeY = 1;
				}
			}
		}

		growtick = 1 + rand() % 3; // 8 + rand() % 12;
	}
	else {
		growtick--;
	}
}
