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
	{ PlayerJetInit, PlayerJetTick, PlayerJetDraw, PlayerJetHurt }, // player
	{ JetBulletInit, JetBulletTick, JetBulletDraw, PlaneHurt }, // jet bullet
	{ TrainInit, TrainTick, TrainDraw, PlaneHurt }, // train
	{ CityPlaneInit, CityPlaneTick, CityPlaneDraw, PlaneHurt }, // xirt plane
	{ NoFunc, E_AlienUfoBulletTick, E_AlienUfoBulletDraw }, // a.ien bullet
	{ E_HunterInit, E_HunterTick, E_HunterDraw, E_UfoHurt }, // hunter

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
		return 2;
	}
	else
	if (angle < 128) {  // |
		return 4;
	}
	else
	if (angle < 150) { //  /
		return 1;
	}
	else {				// <-
		return 5;
	}

}



Entity* GetEntityInDistFlags(int x, int y, int dist, unsigned char flags) {
	
	int _ch;
	Entity* _ent;

	int _chx = ((x - CHUNK_SIZE * 4 ) >> 3) / CHUNK_SIZE;
	int _chy = ((y - CHUNK_SIZE * 4) >> 3) / CHUNK_SIZE;

	for (int ix = std::max(_chx, 0); ix < std::min(_chx + 2, (LEVEL_W / CHUNK_SIZE) - 1); ix++) {
		for (int iy = std::max(_chy, 0); iy < std::min(_chy + 2, (LEVEL_H / CHUNK_SIZE) - 1); iy++) {

			_ch = ix + iy * (LEVEL_W / CHUNK_SIZE);

			for (int i = 0; i < LEVEL.arrChunks[_ch].lsEntities.size(); i++) {
				_ent = LEVEL.arrChunks[_ch].lsEntities[i];
				if (!(_ent->flags & flags)) continue;

				if (abs(_ent->x - x) < dist && abs(_ent->y - y) < dist) {
					return _ent;
				}
			}

		}
	}

	return NULL;

}


Entity* GetEntityInTileFlags(int x, int y, unsigned char flags) {
	int _chunk = (x / CHUNK_SIZE) + (y / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE);
	Entity* _ent;

	if (_chunk < 0 || _chunk >(LEVEL_W / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE)) return NULL;

	for (int i = 0; i < LEVEL.arrChunks[_chunk].lsEntities.size(); i++) {
		_ent = LEVEL.arrChunks[_chunk].lsEntities[i];
		if (!(_ent->flags & flags)) continue;

		if (_ent->x >> 3 == x && _ent->y >> 3 == y) return _ent;

	}

	return NULL;
}



bool PosIsOnScreen(int x, int y) {	
	if (x < CAMERA_X || y < CAMERA_Y || x > CAMERA_X + SCREEN_W || y > CAMERA_Y + SCREEN_H) return false;
	return true;
}
