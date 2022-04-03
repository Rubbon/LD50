#include "Entity.h"
#include "EnemyEntities.h"
#include "Game.h"

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
	if ((_ent->x / CHUNK_SIZE) + (_ent->y / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE) != _ent->currentChunk) {
		//remove from old, add to new
		LEVEL.RemoveEntityFromChunk(_ent, &LEVEL.arrChunks[_ent->currentChunk]);

		//set chunk to the new one
		_ent->currentChunk = (_ent->x / CHUNK_SIZE) + (_ent->y / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE);
		LEVEL.AddEntityToChunk(_ent);
	}
};