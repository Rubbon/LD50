#include "Graphics.h"
#include "SDL_image.h"
#include <iostream>
#include <string>

//screen base info
int Graphics::SCREEN_W = 320;
int Graphics::SCREEN_H = 240;
int Graphics::WINDOW_W = 0;
int Graphics::WINDOW_H = 0;
int Graphics::SCREEN_SCALE = 4;
bool Graphics::FULLSCREEN = false;

SDL_Window* Graphics::window;
SDL_Renderer* Graphics::renderer;

unsigned short Graphics::FPS_CAP = 60;


int Graphics::CAMERA_X = 0;
int Graphics::CAMERA_Y = 0;

int Graphics::GAMESPACE_X = 0;
int Graphics::GAMESPACE_Y = 0;
int Graphics::GAMESPACE_W = 80;
int Graphics::GAMESPACE_H = 80;


SDL_Texture* Graphics::tex_charSet;
//SDL_Texture* Graphics::tex_world;
SDL_Texture* Graphics::tex_fontSet;
SDL_Texture* Graphics::rtex_gameScreen;


//todo - set tileW & tileH based on loaded texture dimensions
void Graphics::LoadGfx() {
	tex_charSet = LoadTexture("res/texChars.png");
	//tex_world = LoadTexture("res/texWorld.png");
	tex_fontSet = LoadTexture("res/texFont.png");

	//generate the texture we're gonna render the gameworld to
	rtex_gameScreen = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGB332, SDL_TEXTUREACCESS_TARGET, GS_W, GS_H);

}

void Graphics::CleanGfx() {
	SDL_DestroyTexture(tex_charSet);
	//SDL_DestroyTexture(tex_world);
	SDL_DestroyTexture(tex_fontSet);

	SDL_DestroyTexture(rtex_gameScreen);
}

SDL_Texture* Graphics::LoadTexture(std::string file) {
	SDL_Surface* _texSurface;
	_texSurface = IMG_Load(file.c_str());
	SDL_Texture* _tex = SDL_CreateTextureFromSurface(renderer, _texSurface);
	SDL_FreeSurface(_texSurface);

	if (_tex == NULL) std::cout << "GFX: [ERROR] Failed to load '" << file << "'!" << std::endl;
	else std::cout << "GFX: Loaded '" << file << "'!" << std::endl;

	return _tex;
}




void Graphics::DrawSpr(SDL_Texture* tex, SDL_Rect drawRect, SDL_Rect srcRect, SDL_Color colour, SDL_RendererFlip flip, double angle, SDL_Point origin) {
	SDL_SetTextureColorMod(tex, colour.r, colour.g, colour.b);
	SDL_SetTextureAlphaMod(tex, colour.a);
	SDL_RenderCopyEx(renderer, tex, &srcRect, &drawRect, NULL, NULL, flip);
}

void Graphics::DrawText(int x, int y, std::string txt, float scale, SDL_Color _col) {
	int _w = 8 * scale;

	SDL_Rect _rect = { 0, y, _w, 8 * scale };
	SDL_Rect _charSpr = { 0, 0, 8, 8 };

	for (int i = 0; i < txt.size(); i++) {
		//special functional stuff
		if (txt[i] == ';') {
			//set colour
			switch (txt[i + 1]) {
			case 'f': // change fg col ;f255,255,255
				_col.r = std::stoi(txt.substr(i + 2, 3));
				_col.g = std::stoi(txt.substr(i + 6, 3));
				_col.b = std::stoi(txt.substr(i + 10, 3));
				txt.replace(i, 13, "");
			break;
			case 'x': // do fx	;x0
				switch (std::stoi(txt.substr(i + 2, 1))) {
					case 0: // colour based on time (fg)
						_col.r = (Uint8)((sin(SDL_GetTicks() * 0.005f) + 1.5f) * 80) % 255;
						_col.g = (Uint8)((sin(100 + SDL_GetTicks() * 0.001f) + 1.5f) * 80) % 255;
						_col.b = (Uint8)((sin(434 + SDL_GetTicks() * 0.0025f) + 1.5f) * 80) % 255;
					break;
				}
				txt.replace(i, 3, "");
			break;
			}
		}

		//draw text fg
		_rect.x = x + i * _w;
		_charSpr.x = ((txt[i]) % 16) * 8;
		_charSpr.y = ((txt[i]) / 16) * 8;
		SDL_SetTextureColorMod(tex_fontSet, _col.r, _col.g, _col.b);
		SDL_RenderCopyEx(renderer, tex_fontSet, &_charSpr, &_rect, NULL, NULL, SDL_FLIP_NONE);

	}
}


void Graphics::DrawTextBg(int x, int y, std::string txt, float scale, SDL_Color _bgCol, SDL_Color _fgCol) {
	int _w = 8 * scale;

	SDL_Rect _bgRect = { 0, y, _w, 8 * scale };
	SDL_Rect _charSpr = { 0, 0, 8, 8 };

	for (int i = 0; i < txt.size(); i++) {
		//special functional stuff
		if (txt[i] == ';') {
			//set colour
			switch (txt[i + 1]) {
			case 'f': // change fg col ;f255,255,255
				_fgCol.r = std::stoi(txt.substr(i + 2, 3));
				_fgCol.g = std::stoi(txt.substr(i + 6, 3));
				_fgCol.b = std::stoi(txt.substr(i + 10, 3));
				txt.replace(i, 13, "");
				break;
			case 'b': // change bg col ;b255,255,255
				_bgCol.r = std::stoi(txt.substr(i + 2, 3));
				_bgCol.g = std::stoi(txt.substr(i + 6, 3));
				_bgCol.b = std::stoi(txt.substr(i + 10, 3));
				txt.replace(i, 13, "");
				break;
			case 'x': // do fx	;x0
				switch (std::stoi(txt.substr(i + 2, 1))) {
				case 0: // colour based on time (fg)
					_fgCol.r = (Uint8)((sin(SDL_GetTicks() * 0.005f) + 1.5f) * 80) % 255;
					_fgCol.g = (Uint8)((sin(100 + SDL_GetTicks() * 0.001f) + 1.5f) * 80) % 255;
					_fgCol.b = (Uint8)((sin(434 + SDL_GetTicks() * 0.0025f) + 1.5f) * 80) % 255;
					break;
				case 1: // colour based on time (bg)
					_bgCol.r = (Uint8)((sin(100 + SDL_GetTicks() * 0.005f) + 1.f) * 30) % 255;
					_bgCol.g = (Uint8)((sin(434 + SDL_GetTicks() * 0.001f) + 1.f) * 30) % 255;
					_bgCol.b = (Uint8)((sin(SDL_GetTicks() * 0.0025f) + 1.f) * 30) % 255;
					break;
				}
				txt.replace(i, 3, "");
				break;
			}
		}

		//draw text bg
		_bgRect.x = x + i * _w;
		SDL_SetRenderDrawColor(renderer, _bgCol.r, _bgCol.g, _bgCol.b, _bgCol.a);
		SDL_RenderFillRect(renderer, &_bgRect);
		//draw text fg
		_charSpr.x = ((txt[i]) % 16) * 8;
		_charSpr.y = ((txt[i]) / 16) * 8;
		SDL_SetTextureColorMod(tex_fontSet, _fgCol.r, _fgCol.g, _fgCol.b);
		SDL_RenderCopyEx(renderer, tex_fontSet, &_charSpr, &_bgRect, NULL, NULL, SDL_FLIP_NONE);

	}
}


void Graphics::DrawRect(SDL_Rect rect, SDL_Color colour) {
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderFillRect(renderer, &rect);
}


void Graphics::DrawLine(int x1, int y1, int x2, int y2, SDL_Colour colour) {
	SDL_SetRenderDrawColor(renderer, colour.r, colour.g, colour.b, colour.a);
	SDL_RenderDrawLine(renderer, x1, y1, x2, y2);
}



SDL_DisplayMode Graphics::GetDisplayInfo(int displayIndex) {
	SDL_DisplayMode _displayMode;
	SDL_GetCurrentDisplayMode(displayIndex, &_displayMode);
	return _displayMode;
}