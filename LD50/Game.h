#pragma once
#include "Entity.h"
#include <vector>
#include "SDL.h"
#include "Level.h"
#include "AlienAi.h"
#include "Sound.h"
#include "Graphics.h"

const SDL_Colour C_XRED = { 255, 64, 64 };
const SDL_Colour C_YBLUE = { 32, 32, 255 };


enum CursorState {
	CS_POINTER,
	CS_DRAG,
	CS_NOCURSOR,
	CS_BUILD_TILE,
	CS_CROSSHAIR,
};

enum GameState {
	GS_BUILD_HQ,
	GS_PLAY,
	GS_BUILD,
	GS_GAMEOVER,
};


class Game {
public:



	Level currentLevel = {};
	unsigned char cursorState = CS_POINTER;

	GameState state = GS_PLAY;

	int playerCash = 400;
	short playerMissiles = 0;
	short numberOfScanners = 0;

	//build menu
	char bm_selected_opt = -1;
	char bm_hover = -1;

	unsigned char mouseInMenu = 0;

	bool playerIsAtHQ = false;

	//border
	short borderTopSize = 16;
	short borderBottomSize = 16;


	//music
	AudioSource audio_bgm;
	bool mute_music = false;

	//newsticker
	int newsticker_time = -SCREEN_W;
	std::string newsticker_text = "";
	//std::vector<std::string> vNewsBacklog;


#define BUILD_OPTIONS 10
	char arrBuildOptions[BUILD_OPTIONS] = {
		TT_AA_GUN,
		TT_AIRFIELD_TL,
		TT_FACTORY_TL,
		TT_WALL,
		TT_CITYSTARTER,
		TT_TRAIN_DEPOT,
		TT_RAIL_STATION_H,
		TT_RAIL_TRACK,
		TT_SCANNER,
		TT_NONE,
	};

	int bm_startX = SCREEN_W - 16 - (BUILD_OPTIONS * 24);

	//building tiles
	TileType tileToBuild = TT_NONE;

	//alien ai controller
	AlienMastermind alienMastermind = {};


	Entity* playerJet = NULL;

	int jetBuildTimer = 128;


	int hovered_tile_x = 0;
	int hovered_tile_y = 0;

	long int gameTick = 0;

	void Init();
	void Tick();
	void Draw();

	void TickCamMovement();

	void DrawUi();
	void SetMusicTo(int sound);

	void AddNews(std::string _txt);

};



extern bool RUN_GAME;
extern Game GAME;

#define GAME_TICK GAME.gameTick
#define LEVEL GAME.currentLevel