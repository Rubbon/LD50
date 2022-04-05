#include <iostream>
#include "SDL.h"
#include "Graphics.h"
#include "Input.h"
#include "Sound.h"
#include "Game.h"

int main(int argc, char* argv[]) {

	//sdl init
	if (SDL_Init(SDL_INIT_EVERYTHING) == 0) {

		//set window scale to something appropriate
		SDL_DisplayMode _dm = Graphics::GetDisplayInfo();

		Graphics::SCREEN_SCALE = std::max((int)((_dm.h / SCREEN_H) * 0.5), 1);

		Graphics::WINDOW_W = SCREEN_W * Graphics::SCREEN_SCALE;
		Graphics::WINDOW_H = SCREEN_H * Graphics::SCREEN_SCALE;

		//Graphics::WINDOW_W = _dm.w * 0.5;
		//Graphics::WINDOW_H = _dm.h * 0.5;

		//SCREEN_W = Graphics::WINDOW_W / Graphics::SCREEN_SCALE;
		//SCREEN_H = Graphics::WINDOW_H / Graphics::SCREEN_SCALE;

		//create window
		Graphics::window = SDL_CreateWindow("LD50 UFO", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, Graphics::WINDOW_W, Graphics::WINDOW_H, SDL_WINDOW_RESIZABLE);
		SDL_SetWindowMinimumSize(Graphics::window, 320, 240);

		//create renderer
		//TODO - do this in a graphics function?
		Graphics::renderer = SDL_CreateRenderer(Graphics::window, -1, SDL_RENDERER_ACCELERATED);
		if (Graphics::renderer) {
			SDL_SetRenderDrawColor(Graphics::renderer, 0, 0, 0, 255);
			SDL_RenderSetLogicalSize(Graphics::renderer, Graphics::SCREEN_W, Graphics::SCREEN_H);
			SDL_RenderSetIntegerScale(Graphics::renderer, SDL_TRUE);
			SDL_SetRenderDrawBlendMode(Graphics::renderer, SDL_BLENDMODE_BLEND);
			SDL_ShowCursor(false);
		}

		std::cout << "+-----------------------+\n| TITLE HERE - Ver 0    |\n|                       |\n| For LD50, 2022        |\n| By Robin Field        |\n| And Billy Hobson      |\n|                       |\n| Thanks for playing!   |\n+-----------------------+" << std::endl;
		std::cout << std::endl << std::endl << std::endl;
		RUN_GAME = true;
	}


	Graphics::LoadGfx();
	Sound::InitSoundDevice();
	Sound::LoadGameAudio();
	GAME.Init();

	SDL_Event sdlEvent;

	//for frame timing
	Uint32 frameStart;
	int frameTime;

	//game loop :)
	while (RUN_GAME) {
		//FRAME START
		frameStart = SDL_GetTicks();

		Input::Tick();
		Sound::Tick();

		while (SDL_PollEvent(&sdlEvent)) {
			switch (sdlEvent.type) {
				case SDL_QUIT:
					RUN_GAME = false;
				break;
				case SDL_WINDOWEVENT: {
					switch (sdlEvent.window.event) {
						case SDL_WINDOWEVENT_SIZE_CHANGED: {
							std::cout << "WINDOW RESIZED!" << std::endl;
							int _winW, _winH;
							SDL_GetWindowSize(Graphics::window, &_winW, &_winH);

							//make sure its not too wide
							if (_winW > _winH * 3) {
								_winW = _winH * 3;
								SDL_SetWindowSize(Graphics::window, _winW, _winH);
							}

							Graphics::SCREEN_SCALE = _winH / 240;
							std::cout << "SCREEN SCALE IS NOW " << Graphics::SCREEN_SCALE << std::endl;
							Graphics::WINDOW_W = _winW;
							Graphics::WINDOW_H = _winH;
							SCREEN_W = _winW / Graphics::SCREEN_SCALE;
							SCREEN_H = _winH / Graphics::SCREEN_SCALE;
							SDL_RenderSetLogicalSize(Graphics::renderer, SCREEN_W, SCREEN_H);
						break; }
					}
				break; }
			}
		}

		//-----------------------------------------------------------------------------------------------------------------------------------------//
		// TICK
		//---------------------------------------------------------------------------------------------------------------------------------------//

		if (Input::KeyPressed(SDL_SCANCODE_ESCAPE)) {
			RUN_GAME = false;
		}

		GAME.Tick();

		//-----------------------------------------------------------------------------------------------------------------------------------------//
		// DRAW
		//---------------------------------------------------------------------------------------------------------------------------------------//
		SDL_SetRenderDrawColor(Graphics::renderer, 160, 160, 174, 255);
		SDL_SetTextureColorMod(Graphics::tex_charSet, 255, 255, 255);
		SDL_SetTextureAlphaMod(Graphics::tex_charSet, 255);
		SDL_RenderClear(Graphics::renderer);

		GAME.Draw();

		//draw end
		SDL_RenderPresent(Graphics::renderer);

		// FRAME END
		//this stuff makes sure that if we finish the frame quicker than the target ms, 
		// it'll stall until it's time for the next frame
		frameTime = SDL_GetTicks() - frameStart;

		//wait
		if ((1000 / Graphics::FPS_CAP) > frameTime) {
			SDL_Delay((1000 / Graphics::FPS_CAP) - frameTime);
		}

		//delta time
		//Input::deltaTime = (SDL_GetTicks() - frameStart) / 1000.0f;

	}


	//Cleanup stuff that won't free itself
	Graphics::CleanGfx();
	Sound::CleanupSoundDevice();
	//Game::Cleanup();
	SDL_DestroyWindow(Graphics::window);
	SDL_DestroyRenderer(Graphics::renderer);
	SDL_Quit();
	return 0;
}
