#include "Entity.h"
#include "EnemyEntities.h"
#include "Game.h"
#include <iostream>

const EntityFunctions arrEntityFuncs[] = {
	//Init, Tick, Draw, onhurt, onhurtother
	{ E_UfoInit, E_UfoTick, E_UfoDraw }, // ufo
	{ E_WalkerInit, E_WalkerTick, E_WalkerDraw }, // walker

};


void NoFunc(Entity* _entity) {
	return;
};

void NoFunc(Entity* _entity_1, Entity* _entity_2) {
	return;
}






void SortEntityIntoCorrectChunk(Entity* _ent) {
	if (GetChunkIndexAtEntityPos(_ent->x, _ent->y) != _ent->currentChunk) {
		//remove from old, add to new
		LEVEL.RemoveEntityFromChunk(_ent, &LEVEL.arrChunks[_ent->currentChunk]);

		//set chunk to the new one
		_ent->currentChunk = GetChunkIndexAtEntityPos(_ent->x, _ent->y);
		LEVEL.AddEntityToChunk(_ent);

		//todo - remove me
		std::cout << "SORTED ENTITY #" << _ent->id << " INTO CHUNK " << _ent->currentChunk << std::endl;

	}
}


short GetChunkIndexAtEntityPos(int x, int y) {
	return ((x>>3) / CHUNK_SIZE) + ((y>>3) / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE);
}

;