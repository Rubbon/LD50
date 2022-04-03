#include "Level.h"
#include <iostream>
#include <fstream>
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
	int i;


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
			}


		}
	}

	//for positions we can successfully check to place stuff
	std::vector<Pos> vPositionsWeCanCheck;


	//fill out tilemap
	for (i = 0; i < w * h; i++) {
		if (height[i] > 0.33f && ((height[i - 1] > 0.33f || height[i + 1] > 0.33f) && (height[i - w] > 0.33f || height[i + w] > 0.33f))) {
			level->arrTiles[i].type = TT_LAND;

			if (i % LEVEL_W > 6 && i % LEVEL_W < LEVEL_W - 6 && i / LEVEL_H > 4 && i / LEVEL_H < LEVEL_H-4) vPositionsWeCanCheck.push_back({ (short)(i % LEVEL_W), (short)(i / LEVEL_W) });
		}

		if (height[i] >= 0.36f && height[i] < 0.363f) level->arrTiles[i].type = TT_TREE;
		//if (height[i] >= 0.34f && height[i] < 0.35f) level->arrTiles[i].type = TT_TREE;

	}
	



	Pos _pos;
	Tile* _t;

	//lets make some cities
	int _cityAmt = 4 + (rand() % MAX_CITIES - 4);


	//city names
	std::vector<std::string> prefixes;
	std::vector<std::string> suffixes;
	std::fstream townstxt;
	townstxt.open("res/towns.txt", std::ios::in);
	if (townstxt.is_open()) {
		std::string _line;
		bool _endOfPref = false;
		while (std::getline(townstxt, _line)) {
			if (_line == "") {
				_endOfPref = true;
				continue;
			}
			if (!_endOfPref) prefixes.push_back(_line);
			else suffixes.push_back(_line);
		}
		townstxt.close();
	}
	else std::cout << "No towns.txt found!" << std::endl;

	//for (int i = 0; i < prefixes.size(); i++) std::cout << prefixes[i] << std::endl;
	//for (int i = 0; i < suffixes.size(); i++) std::cout << suffixes[i] << std::endl;

	for (i = 0; i < _cityAmt; i++) {
		//get a position to check
		int _posi = rand()%vPositionsWeCanCheck.size();
		_pos = vPositionsWeCanCheck[_posi];

		_t = level->GetTile(_pos.x, _pos.y);

		//abort if cant build here
		if (_t->type == TT_CITYBLOCK_BIG || _t->type == TT_CITYBLOCK_SMALL) {
			vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
			i--;
			continue;
		}

		//set city data
		level->arrCities[i].flags = CF_ACTIVE;
		level->arrCities[i].origin_x = _pos.x;
		level->arrCities[i].origin_y = _pos.y;
		std::string _cityName;
		_cityName = prefixes[(int)rand() % prefixes.size()]+suffixes[(int)rand() % suffixes.size()];
		level->arrCities[i].name = _cityName;

		PreGenerateCity(level, i);

		std::cout << "[GEN] added city at " << _pos.x << ", " << _pos.y << std::endl;

		//remove the check position when done
		vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
	}



}



void LevelGenerator::PreGenerateCity(Level* level, int city_i) {
	int _growthAmt = 8 + rand() % 16;

	City* _city = &level->arrCities[city_i];
	Tile* _t;

	int _placeX = _city->origin_x;
	int _placeY = _city->origin_y;

	int _maxTries = 64; // for safety


	for (int i = 0; i < _growthAmt*2; i++) {
		
		//safety
		if (_maxTries <= 0) return;

		_t = level->GetTile(_placeX, _placeY);

		if (_t->type == TT_LAND) {

			//type
			TileType _cityBlockType = TT_CITYBLOCK_BIG;
			if (i > _growthAmt) _cityBlockType = TT_CITYBLOCK_SMALL;


			_t->type = _cityBlockType;
			_t->owner = city_i;
			_maxTries++;

			//reset to start
			_placeX = _city->origin_x;
			_placeY = _city->origin_y;

		} else {
			//move
			_placeX += (rand() % 3) - 1;
			_placeY += (rand() % 3) - 1;
			if (_placeX == 0 && _placeY == 0) {
				_placeX = -1;
				_placeY = 1;
			}
		}

		_maxTries--;

	}

}
