#include "Entity.h"
#include "EnemyEntities.h"
#include "MiscEntities.h"
#include "Game.h"
#include <iostream>
#include "PlayerEntities.h"

const EntityFunctions arrEntityFuncs[] = {
	//Init, Tick, Draw, onhurt, onhurtother
	{ E_UfoInit, E_UfoTick, E_UfoDraw, E_UfoHurt }, // ufo
	{ E_WalkerInit, E_WalkerTick, E_WalkerDraw, E_UfoHurt }, // walker
	{ FxInit, FxTick, FxDraw }, // fx
	{ PlayerJetInit, PlayerJetTick, PlayerJetDraw }, // player
	{ JetBulletInit, JetBulletTick, JetBulletDraw }, // jet bullet
	{ TrainInit, TrainTick, TrainDraw }, // train

};


void NoFunc(Entity* _entity) {
	return;
};

void NoFunc(Entity* _entity_1, Entity* _entity_2) {
	return;
}






//FX FUNCTIONS
Entity* SpawnFx(int x, int y, int z, int lifetime, unsigned char stateFlags) {
	Entity* _returnEnt = LEVEL.AddEntity(x, y, ENT_FX);
	_returnEnt->z = z;
	_returnEnt->state = stateFlags;
	_returnEnt->hp = lifetime;
	return _returnEnt;
}

void SetFxMotion(Entity* ent, float xspd, float yspd, float zspd) {
	ent->mx = xspd;
	ent->my = yspd;
	ent->mz = zspd;
}

void SetFxSpr(Entity* ent, SDL_Rect sprRect, SDL_Colour blend) {
	ent->animSpr = sprRect;
	ent->blend = blend;
}




void SortEntityIntoCorrectChunk(Entity* _ent) {
	if (GetChunkIndexAtEntityPos(_ent->x, _ent->y) != _ent->currentChunk) {
		//remove from old, add to new
		LEVEL.RemoveEntityFromChunk(_ent);// , &LEVEL.arrChunks[_ent->currentChunk]);

		//set chunk to the new one
		_ent->currentChunk = GetChunkIndexAtEntityPos(_ent->x, _ent->y);
		LEVEL.AddEntityToChunk(_ent);

		//todo - remove me
		//std::cout << "SORTED ENTITY #" << _ent->id << " INTO CHUNK " << _ent->currentChunk << std::endl;

	}
}


short GetChunkIndexAtEntityPos(int x, int y) {
	return ((x>>3) / CHUNK_SIZE) + ((y>>3) / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE);
};



void DeleteEntity(Entity* ent) {
	if (ent->currentChunk != -1) LEVEL.RemoveEntityFromChunk(ent);
	ent->flags |= EFL_DELETED;
}



int GetSprOffsetOnAngle(float angle) {

	angle = abs(round(angle * 180 / M_PI));

	if (angle < 22.5) { // ->
		return 0;
	}
	else
	if (angle < 50) {   // \  
		return 1;
	}
	else
	if (angle < 128) {  // |
		return 2;
	}
	else
	if (angle < 150) { //  /
		return 3;
	}
	else {				// <-
		return 4;
	}

}



Entity* GetEntityInDistFlags(int x, int y, int dist, unsigned char flags) {
	
	int _ch = GetChunkIndexAtEntityPos(x, y);
	Entity* _ent;

	for (int i = 0; i < LEVEL.arrChunks[_ch].lsEntities.size(); i++) {
		_ent = LEVEL.arrChunks[_ch].lsEntities[i];
		if (!(_ent->flags & flags)) continue;
		if (abs(_ent->x - x) < dist || abs(_ent->y - y) < dist) {
			return _ent;
		}
	}

	return NULL;

}
