#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define RECON_SPACE_SIZE 8



struct AlienParty {
	std::vector<Entity*> vEntities;
	short gather_x = 0;
	short gather_y = 0;

	void TellEntitiesToGather();

};


class AlienMastermind {
public:
	//std::vector<Entity*> vActiveAlienUnits;

	short warStage = 0;

	std::vector<AlienParty> vAttackParties;

	//areas recon found that have something in them
	std::vector<Pos> vAreasOfInterest;
	//if we've found hq
	Pos foundHqPos = {-1, -1};

	//recon
	Pos searchLocation = {0,0};
	Pos lastSearchLocation = {0,0};
	Entity* _reconUnit = NULL;


	void Tick();

	void TryDoingRecon();

};