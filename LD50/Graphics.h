#pragma once
#include "SDL.h"
#include <iostream>

class Graphics {
public:
	static int SCREEN_W;
	static int SCREEN_H;
	static int WINDOW_W;
	static int WINDOW_H;
	static int SCREEN_SCALE;
	static bool	FULLSCREEN;

	static SDL_Window* window;
	static SDL_Renderer* renderer;

	static unsigned short FPS_CAP;


	static SDL_Texture* tex_charSet;
	//static SDL_Texture* tex_world;
	static SDL_Texture* tex_fontSet;
	static SDL_Texture* rtex_gameScreen;
	static short TILE_W;
	static short TILE_H;

	static int CAMERA_X;
	static int CAMERA_Y;

	//for drawing gamespace
	static int GAMESPACE_X;
	static int GAMESPACE_Y;
	static int GAMESPACE_W;
	static int GAMESPACE_H;


	static void LoadGfx();
	static void CleanGfx();


	//sdl wrappers
	static SDL_DisplayMode GetDisplayInfo(int displayIndex = 0);


	//drawing functions

	static void DrawSpr(SDL_Texture* tex, SDL_Rect drawRect, SDL_Rect srcRect, SDL_Color colour = {255, 255, 255, 255}, SDL_RendererFlip flip = SDL_FLIP_NONE, double angle = 0, SDL_Point origin = { 0,0 });

	static void DrawText(int x, int y, std::string txt, float scale, SDL_Color _col = {255,255,255,255});
	static void DrawTextBg(int x, int y, std::string txt, float scale, SDL_Color _bgCol, SDL_Color _fgCol);

	static void DrawRect(SDL_Rect rect, SDL_Color colour);

	static void DrawLine(int x1, int y1, int x2, int y2, SDL_Colour colour = {255, 255, 255, 255});

private:

	static SDL_Texture* LoadTexture(std::string file);	

};

#define SCREEN_W Graphics::SCREEN_W
#define SCREEN_H Graphics::SCREEN_H

#define TEX_CHARS Graphics::tex_charSet
//#define TEX_WORLD Graphics::tex_world
#define TEX_FONT Graphics::tex_font

#define GS_W Graphics::GAMESPACE_W
#define GS_H Graphics::GAMESPACE_H

#define CAMERA_X Graphics::CAMERA_X
#define CAMERA_Y Graphics::CAMERA_Y