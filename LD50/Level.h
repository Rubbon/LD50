#pragma once
#include "Entity.h"
#include <vector>
#include "Tile.h"


#define MAX_ENTITIES 2048
#define MAX_CITIES 8

#define LEVEL_W 176
#define LEVEL_H 176

#define CHUNK_SIZE 16
#define CHUNK_FULL_SIZE CHUNK_SIZE * 8



//for quick entity-entity action
struct Chunk {
	std::vector<Entity*> lsEntities;
};


class Level {
public:

	//tiles
	Tile arrTiles[LEVEL_W * LEVEL_H] = {};
	std::vector<Pos> vTilesToTick;

	//chunks
	Chunk arrChunks[(LEVEL_W/CHUNK_SIZE) * (LEVEL_H/CHUNK_SIZE)];

	//entities
	Entity arrEntities[MAX_ENTITIES];
	unsigned int entityIterator;
	//all entities that are active in the level right now
	std::vector<Entity*> vActiveEntities;

	std::vector<Entity*> vActivePlayerUnits;

	Hq playerHq = {};



	City arrCities[MAX_CITIES];

	void Tick();
	void Draw();

	Tile* GetTile(int x, int y);
	Tile* GetTile(Pos pos);

	Entity* AddEntity(int x, int y, unsigned short entityIndex, bool runInit = true);


	void RemoveEntityFromChunk(Entity* entity);
	void AddEntityToChunk(Entity* entity);
	//Chunk* GetChunkAtRealPos(int x, int y);
	Chunk* GetChunkAtTilePos(int x, int y);

	Entity* GetEntityAtTile(int x, int y, unsigned char neededFlags = 0x00);


};


class LevelGenerator {
public:
	float height[LEVEL_W * LEVEL_H];

	const int _noiseSeedSize = LEVEL_W * LEVEL_H;
	float _noiseSeed[LEVEL_W * LEVEL_H];

	void GenerateWorld(Level* level);

	void PreGenerateCity(Level* level, int cityIndex);

	std::string GetCityName();
};