#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define MAX_ENTITIES 512
#define MAX_CITIES 8

#define LEVEL_W 176
#define LEVEL_H 176


class Level {
public:
	Tile arrTiles[LEVEL_W * LEVEL_H] = {};

	//entities
	Entity arrEntities[MAX_ENTITIES];
	unsigned int entityIterator;
	//all entities that are active in the level right now
	std::vector<Entity*> vActiveEntities;

	City arrCities[MAX_CITIES];


	Tile* GetTile(int x, int y);

	Entity* AddEntity(int x, int y, unsigned short entityIndex);


};


class LevelGenerator {
public:
	float height[LEVEL_W * LEVEL_H];

	const int _noiseSeedSize = LEVEL_W * LEVEL_H;
	float _noiseSeed[LEVEL_W * LEVEL_H];

	void GenerateWorld(Level* level);

	void PreGenerateCity(Level* level, int cityIndex);

};