#pragma once
#include "SDL.h"

class Input {
public:

	static int MouseX;
	static int MouseY;
	//mouse but relative to screen
	static int cursorX;
	static int cursorY;
	static float deltaTime;
	static int mouseWheelY;

	//lets put keybinds here :)
	static SDL_Scancode KEY_LEFT;
	static SDL_Scancode KEY_RIGHT;
	static SDL_Scancode KEY_UP;
	static SDL_Scancode KEY_DOWN;
	static SDL_Scancode KEY_ATTACK;
	static SDL_Scancode KEY_BOMB;




	static void Tick();

	//KB
	static bool KeyHeld(SDL_Scancode key);
	static bool KeyPressed(SDL_Scancode key);
	static bool KeyReleased(SDL_Scancode key);

	//M
#define MB_LEFT 1
#define MB_MIDDLE 2
#define MB_RIGHT 3

	static bool MouseHeld(unsigned char button);
	static bool MousePressed(unsigned char button);
	static bool MouseReleased(unsigned char button);
	static void SetCursorPos(int x, int y);

private:


};

#define CURSOR_X Input::cursorX
#define CURSOR_Y Input::cursorY