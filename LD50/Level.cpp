#include "Level.h"
#include <iostream>
#include <fstream>
#include "SDL.h"
#include "Game.h"
#include "TileBaseInfo.h"
#include <algorithm>
#include "Graphics.h"

unsigned char cityTick = 0;
int  i;

Tile _noTile = {TT_NONE};

void Level::Tick() {

	//city tick
	if (GAME_TICK % 60 == 0) {
		if (arrCities[cityTick].flags & CF_ACTIVE) arrCities[cityTick].expandTick();
		cityTick++;
		if (cityTick >= MAX_CITIES) cityTick = 0;
	}

	//tile tick
	for (i = 0; i < vTilesToTick.size(); i++) {
		//remove if not tickable anymore
		if (!(GET_TILE_INFO(GetTile(vTilesToTick[i])->type).flags & TIF_TICKABLE)) {
			vTilesToTick.erase(vTilesToTick.begin() + i);
			i--;
			continue;
		}

		TileTick(vTilesToTick[i].x, vTilesToTick[i].y, GetTile(vTilesToTick[i]));

	}

	//force mpty chnks for freedom
	//for (i = 0; i < (LEVEL_W / CHUNK_SIZE) * (LEVEL_H / CHUNK_SIZE); i++) {
	//	arrChunks[i].lsEntities.clear();
	//}

	//tick active entities
	for (i = 0; i < vActiveEntities.size(); i++) {
		//remove from active entity list if deleted
		if (vActiveEntities[i]->flags & EFL_DELETED) {
			//remove from any chunk its in
			//if (vActiveEntities[i]->currentChunk != -1) LEVEL.RemoveEntityFromChunk(vActiveEntities[i], &LEVEL.arrChunks[vActiveEntities[i]->currentChunk]);
			//remove from active duty
			vActiveEntities.erase(vActiveEntities.begin() + i);
			continue;
		}

		//add entities to chunks (depending on how slow, might do this less often)
		SortEntityIntoCorrectChunk(vActiveEntities[i]);
		//vActiveEntities[i]->currentChunk = GetChunkIndexAtEntityPos(vActiveEntities[i]->x, vActiveEntities[i]->y);
		//AddEntityToChunk(vActiveEntities[i]);

		arrEntityFuncs[vActiveEntities[i]->entityIndex].Tick(vActiveEntities[i]);
	}

}



void Level::Draw() {


	//draw current world tiles
	int _ctx = CAMERA_X >> 3;
	int _cty = CAMERA_Y >> 3;
	int ix, iy;

	//draw tiles
	for (ix = _ctx + 1; ix <= _ctx + Graphics::SCREEN_W / 8; ix++) {
		if (ix < 0 || ix >= LEVEL_W) continue;
		for (iy = _cty + 1; iy <= _cty + Graphics::SCREEN_H / 8; iy++) {
			if (iy < 0 || iy >= LEVEL_H) continue;
			TileDraw((ix * 8) - CAMERA_X, (iy * 8) - CAMERA_Y, ix, iy, GetTile(ix, iy));
		}
	}


	//cities draw their name TODO - DON'T DRAW WHEN OUTSIDE SCREEN
	for (int i = 0; i < MAX_CITIES; i++) {

		int _xx = (-arrCities[i].name.length() * 4) + (arrCities[i].origin_x * 8) - CAMERA_X;

		Graphics::DrawText(1 + _xx, 1 + (arrCities[i].origin_y * 8) - CAMERA_Y, arrCities[i].name, 1, { 0, 0, 0, 255 });
		Graphics::DrawText(_xx, (arrCities[i].origin_y * 8) - CAMERA_Y, arrCities[i].name, 1, { 255, 227, 128, 255 });
	}


	//sort entities so they draw in the right order
	//std::sort(vActiveEntities.begin(), vActiveEntities.end(), EntitySorter());


	//old - draws All entities
	//draw active entities
	//for (i = 0; i < vActiveEntities.size(); i++) {
	//	arrEntityFuncs[vActiveEntities[i]->entityIndex].Draw(vActiveEntities[i]);
	//}


	//draw entities in viewable chunks
	_ctx /= CHUNK_SIZE;
	_cty /= CHUNK_SIZE;
	Chunk* _chunk;

	for (ix = _ctx; ix <= 1 + _ctx + (Graphics::SCREEN_W >> 3) / CHUNK_SIZE; ix++) {
		if (ix < 0 || ix >= LEVEL_W / CHUNK_SIZE) continue;
		for (iy = _cty; iy <= 1 + _cty + (Graphics::SCREEN_H >> 3) / CHUNK_SIZE; iy++) {
			if (iy < 0 || iy >= LEVEL_H / CHUNK_SIZE) continue;
			
			_chunk = &arrChunks[ix + iy * (LEVEL_W / CHUNK_SIZE)];

			//sort entities so they draw in the right order (might cause chunk border rendering issues)
			std::sort(_chunk->lsEntities.begin(), _chunk->lsEntities.end(), EntitySorter());

			for (i = 0; i < _chunk->lsEntities.size(); i++) {
				arrEntityFuncs[_chunk->lsEntities[i]->entityIndex].Draw(_chunk->lsEntities[i]);
			}

		}
	}



}



Tile* Level::GetTile(int x, int y) {
	if (x > LEVEL_W || x < 0 || y > LEVEL_H || y < 0) return &_noTile;
	return &arrTiles[x + y * LEVEL_W];
}

Tile* Level::GetTile(Pos pos) {
	return &arrTiles[pos.x + pos.y * LEVEL_W];
}





Entity* Level::AddEntity(int x, int y, unsigned short entityIndex, bool runInit) {
	Entity _entity = {};
	_entity.x = x;
	_entity.y = y;
	_entity.fx = _entity.x;
	_entity.fy = _entity.y;
	_entity.entityIndex = entityIndex;
	_entity.flags &= ~EFL_DELETED;

	//run init function
	if (runInit) arrEntityFuncs[entityIndex].Init(&_entity);

	//find a slot we can put this entity in
	for (i = entityIterator; i < MAX_ENTITIES; i++) {
		if (arrEntities[i].flags & EFL_DELETED) {
			_entity.id = i;
			arrEntities[i] = _entity;
			vActiveEntities.push_back(&arrEntities[i]);
			entityIterator = i;
			return &arrEntities[i];
		}
	}

	//if we can't find a slot this way, go the other way
	for (i = 0; i < entityIterator; i++) {
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

Chunk* Level::GetChunkAtTilePos(int x, int y) {
	return &arrChunks[(x / CHUNK_SIZE) + (y / CHUNK_SIZE) * (LEVEL_W / CHUNK_SIZE)];
}



Entity* Level::GetEntityAtTile(int x, int y, unsigned char neededFlags) {
	Chunk* _chunk = GetChunkAtTilePos(x, y);
	for (int i = 0; i < _chunk->lsEntities.size(); i++) {
		//skip if doesn't have required flags
		if (!(_chunk->lsEntities[i]->flags & neededFlags)) continue;

		if (_chunk->lsEntities[i]->x >> 3 == x && _chunk->lsEntities[i]->y >> 3 == y) {
			return _chunk->lsEntities[i];
		}
	}
	return NULL;
}



void Level::RemoveEntityFromChunk(Entity* ent) {
	//auto _posInChunk = std::find(_chunk->lsEntities.begin(), _chunk->lsEntities.end(), ent);
	//if (_posInChunk != std::end(_chunk->lsEntities)) _chunk->lsEntities.erase(_posInChunk);

	if (ent->currentChunk < 0 || ent->currentChunk >= (LEVEL_W / CHUNK_SIZE) * (LEVEL_H / CHUNK_SIZE)) return;

	//doing it my own way to aovid crashes?
	for (int i = 0; i < arrChunks[ent->currentChunk].lsEntities.size(); i++) {
		if (arrChunks[ent->currentChunk].lsEntities[i]->id == ent->id) {
			arrChunks[ent->currentChunk].lsEntities.erase(arrChunks[ent->currentChunk].lsEntities.begin() + i);
			return;
		}
	}

}


void Level::AddEntityToChunk(Entity* ent) {
	arrChunks[ent->currentChunk].lsEntities.push_back(ent);
}



std::string LevelGenerator::GetCityName() {

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
	else {
		std::cout << "No towns.txt found!" << std::endl;
		prefixes.push_back("There's no towns.txt file, ");
		suffixes.push_back("dummy.");
		suffixes.push_back("idjit.");
		suffixes.push_back("buffoon.");
		suffixes.push_back("clown.");
	}

	//for (int i = 0; i < prefixes.size(); i++) std::cout << prefixes[i] << std::endl;
	//for (int i = 0; i < suffixes.size(); i++) std::cout << suffixes[i] << std::endl;

	std::string _cityName;
	_cityName = prefixes[(int)rand() % prefixes.size()] + suffixes[(int)rand() % suffixes.size()];
	return _cityName;
}




void LevelGenerator::GenerateWorld(Level* level) {

	const int w = LEVEL_W;
	const int h = LEVEL_H;


	srand(time(NULL)%32);

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
	int _posi;

	//lets make some cities
	int _cityAmt = 4 + rand() % (MAX_CITIES - 4);

	for (i = 0; i < _cityAmt; i++) {

		//find a good spot to put the city
		bool _foundAdequateSpot = false;

		while (!_foundAdequateSpot) {

			//get a position to check
			_posi = rand() % vPositionsWeCanCheck.size();
			_pos = vPositionsWeCanCheck[_posi];

			_t = level->GetTile(_pos.x, _pos.y);

			//abort if cant build here
			if (_t->type == TT_CITYBLOCK_BIG || _t->type == TT_CITYBLOCK_SMALL) {
				vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
			} else {
				_foundAdequateSpot = true;
			}

		}


		//set city data
		level->arrCities[i].flags = CF_ACTIVE;
		level->arrCities[i].origin_x = _pos.x;
		level->arrCities[i].origin_y = _pos.y;
		level->arrCities[i].name = GetCityName();
		level->arrCities[i].index = i;

		PreGenerateCity(level, i);

		std::cout << "[GEN] added city at " << _pos.x << ", " << _pos.y << std::endl;

		//remove the check position when done
		vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
	}


	//gen some factories

	int _factoryAmt = 4 + rand() % (MAX_CITIES - 4);

	for (i = 0; i < _factoryAmt; i++) {
		//get a position to check
		_posi = rand() % vPositionsWeCanCheck.size();
		_pos = vPositionsWeCanCheck[_posi];

		_t = level->GetTile(_pos.x, _pos.y);

		//abort if cant build here
		if (_t->type == TT_CITYBLOCK_BIG || _t->type == TT_CITYBLOCK_SMALL) {
			vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
			i --;
			continue;
		}

		//don't build if gonna build on top of something
		if (GET_TILE_INFO(level->GetTile(_pos.x + 1, _pos.y)->type).flags & TIF_HUMAN || GET_TILE_INFO(level->GetTile(_pos.x + 1, _pos.y + 1)->type).flags & TIF_HUMAN || GET_TILE_INFO(level->GetTile(_pos.x, _pos.y + 1)->type).flags & TIF_HUMAN) {
			continue;
		}


		//spawqn factory
		_t->type = TT_FACTORY_TL;
		_t->hp = GET_TILE_INFO(TT_FACTORY_TL).baseHp;

		_t = level->GetTile(_pos.x + 1, _pos.y);
		_t->type = TT_FACTORY_TR;
		_t->hp = GET_TILE_INFO(TT_FACTORY_TR).baseHp;

		_t = level->GetTile(_pos.x, _pos.y + 1);
		_t->type = TT_FACTORY_BL;
		_t->hp = GET_TILE_INFO(TT_FACTORY_BL).baseHp;

		_t = level->GetTile(_pos.x + 1, _pos.y + 1);
		_t->type = TT_FACTORY_BR;
		_t->hp = GET_TILE_INFO(TT_FACTORY_BR).baseHp;


		//remove the check position when done
		vPositionsWeCanCheck.erase(vPositionsWeCanCheck.begin() + _posi);
	}

}



void LevelGenerator::PreGenerateCity(Level* level, int city_i) {
	int _growthAmt = 8 + rand() % 32;

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
			_city->myTiles.push_back({ (short)_placeX, (short)_placeY});
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
