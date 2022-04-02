#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define MAX_ENTITIES 512

#define LEVEL_W 256
#define LEVEL_H 256


class Level {
public:
	Tile arrTiles[LEVEL_W * LEVEL_H] = {};

	//entities
	Entity arrEntities[MAX_ENTITIES];
	unsigned int entityIterator;
	//all entities that are active in the level right now
	std::vector<Entity*> vActiveEntities;

	Tile* GetTile(int x, int y);

	Entity* AddEntity(int x, int y, unsigned short entityIndex);


	void GenerateWorld();



};