#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define RECON_SPACE_SIZE 8


class AlienMastermind {
public:
	std::vector<Entity*> vActiveAlienUnits;

	short warStage = 0;


	//areas recon found that have something in them
	std::vector<Pos> vAreasOfInterest;
	//cities we've found
	std::vector<unsigned char> vFoundCities;
	//if we've found hq
	Pos foundHqPos = {-1, -1};

	//recon
	Pos searchLocation = {0,0};
	Pos lastSearchLocation = {0,0};
	Entity* _reconUnit = NULL;


	void Tick();

};