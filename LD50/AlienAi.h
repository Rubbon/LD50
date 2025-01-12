#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define RECON_SPACE_SIZE 16


struct AlienParty {
	std::vector<Entity*> vEntities;
	short gather_x = 0;
	short gather_y = 0;

	int start_time = 0;

	//int hangAroundTimer = 0;

	//short target_x = 0;
	//short target_y = 0;

	void TellEntitiesToGather();

};


struct AlienHive {
	std::vector<Pos> vTiles;
	int growtick = 0;
	int index = 0;
	short origin_x = 0;
	short origin_y = 0;

	void Grow();
};


class AlienMastermind {
public:
	std::vector<Entity*> vActiveAlienUnits;

	short warStage = 1;

	std::vector<AlienParty> vAttackParties;
	std::vector<AlienHive> vHives;

	//areas recon found that have something in them
	std::vector<Pos> vAreasOfInterest;
	std::vector<Pos> vEmptyPlots;
	//if we've found hq
	Pos foundHqPos = {-1, -1};

	//recon
	Pos searchLocation = {0,0};
	Pos lastSearchLocation = {0,0};
	std::vector<Pos> lastSearchedLocations = {};
	//Entity* _reconUnit = NULL;
	std::vector<Entity*> vReconUnits;


	Entity* _helpessMan = NULL;

	void Tick();

	void TryDoingRecon();
	void TryDoingAttack();

};