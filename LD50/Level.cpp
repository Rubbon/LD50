#include "Level.h"
#include <iostream>
#include "SDL.h"



Tile* Level::GetTile(int x, int y) {
	return &arrTiles[x + y * LEVEL_W];
}






Entity* Level::AddEntity(int x, int y, unsigned short entityIndex) {
	Entity _entity = {};
	_entity.x = x;
	_entity.y = y;
	_entity.entityIndex = entityIndex;
	_entity.flags &= ~EFL_DELETED;

	//run init function
	//arrEntityFuncs[entityIndex].Init(&_entity);

	//find a slot we can put this entity in
	for (int i = entityIterator; i < MAX_ENTITIES; i++) {
		if (arrEntities[i].flags & EFL_DELETED) {
			_entity.id = i;
			arrEntities[i] = _entity;
			vActiveEntities.push_back(&arrEntities[i]);
			entityIterator = i;
			return &arrEntities[i];
		}
	}

	//if we can't find a slot this way, go the other way
	for (int i = 0; i < entityIterator; i++) {
		if (arrEntities[i].flags & EFL_DELETED) {
			_entity.id = i;
			arrEntities[i] = _entity;
			vActiveEntities.push_back(&arrEntities[i]);
			entityIterator = i;
			return &arrEntities[i];
		}
	}

	//there's too many entities
	//todo - might need to handle when there's too many
	puts("[ERROR] TOO MANY ENTITIES!!");

	//return &arrEntities[entityIterator];
}






void LevelGenerator::GenerateWorld(Level* level) {

	const int w = LEVEL_W;
	const int h = LEVEL_H;

	srand(time(NULL)%16);

	for (int i = 0; i < _noiseSeedSize; i++) {
		_noiseSeed[i] = (float)rand() / (float)RAND_MAX;
	}

	int ix, iy, o;
	int _octaves = 6; // depth of the generation

	for (ix = 0; ix < w; ix++) {
		for (iy = 0; iy < h; iy++) {
			float noise = 0.0f;
			float scale = 1.5f;
			float _scaleAcc = 0.0f;

			for (o = 0; o < _octaves; o++) {
				int pitch = w >> o;
				int sample_x1 = (ix / pitch) * pitch;
				int sample_y1 = (iy / pitch) * pitch;

				int sample_x2 = (sample_x1 + pitch) % w;
				int sample_y2 = (sample_y1 + pitch) % w;

				float blendX = (float)(ix - sample_x1) / (float)pitch;
				float blendY = (float)(iy - sample_y1) / (float)pitch;

				float sampleT = (1.0f - blendX) * _noiseSeed[(sample_y1 * w + sample_x1) % _noiseSeedSize] + blendX * _noiseSeed[(sample_y1 * w + sample_x2) % _noiseSeedSize];
				float sampleB = (1.0f - blendX) * _noiseSeed[(sample_y2 * w + sample_x1) % _noiseSeedSize] + blendX * _noiseSeed[(sample_y2 * w + sample_x2) % _noiseSeedSize];

				noise += (blendY * (sampleB - sampleT) + sampleT) * scale;
				_scaleAcc += scale;
				scale = scale * 0.75f; // bias
				height[ix + iy * w] = noise / _scaleAcc;

				//if (noise / _scaleAcc > 1) {
				//	arrTiles[ix + iy * w].type = TT_LAND;
				//}

			}


		}
	}


	
	for (int i = 0; i < w * h; i++) {
		if (height[i] > 0.33f && (height[i - 1] > 0.33f || height[i - w] > 0.33 || height[i + 1] > 0.33f || height[i + w] > 0.33f)) {
			level->arrTiles[i].type = TT_LAND;
		}
	}
	

	/*
	for (int ix = 0; ix < w; ix++) {
		for (int iy = 0; iy < h; iy++) {
			if (height[ix + iy * w] > 0.33f) {
				arrTiles[((ix*2) + (iy*2) * LEVEL_W)].type = TT_LAND;
				arrTiles[((1+ix*2) + (iy*2) * LEVEL_W)].type = TT_LAND;
				arrTiles[((ix*2) + (1+iy*2) * LEVEL_W)].type = TT_LAND;
				arrTiles[((1+ix*2) + (1+iy*2) * LEVEL_W)].type = TT_LAND;
			}
		}
	}
	*/


}
