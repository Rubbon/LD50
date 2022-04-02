#include "Input.h"
#include <iostream>
#include "Graphics.h"

int Input::MouseX = 0;
int Input::MouseY = 0;
int Input::cursorX;
int Input::cursorY;
float Input::deltaTime = 0;
int Input::mouseWheelY;

//key bindings
SDL_Scancode Input::KEY_LEFT = SDL_SCANCODE_LEFT;
SDL_Scancode Input::KEY_RIGHT = SDL_SCANCODE_RIGHT;
SDL_Scancode Input::KEY_UP = SDL_SCANCODE_UP;
SDL_Scancode Input::KEY_DOWN = SDL_SCANCODE_DOWN;
SDL_Scancode Input::KEY_ATTACK = SDL_SCANCODE_Z;
SDL_Scancode Input::KEY_BOMB = SDL_SCANCODE_X;


const Uint8* keyState = SDL_GetKeyboardState(NULL);
bool prevKeyState[322] = { false };


//for active mouse button
bool lastMouseButton[3] = { false };

void Input::Tick() {

	//set previous key info (only loop through buttons used?)
	for (int i = 0; i < 322; i++) {
		prevKeyState[i] = keyState[i];
	}

	SDL_GetMouseState(&MouseX, &MouseY);

	//set previous mouse button info
	for (int i = 0; i < 3; i++) {
		lastMouseButton[i] = SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(i);
	}

	cursorX = (MouseX / Graphics::SCREEN_SCALE);
	cursorY = (MouseY / Graphics::SCREEN_SCALE);

	mouseWheelY = 0;
}


//KB

bool Input::KeyHeld(SDL_Scancode key) {
	return keyState[key];
}

bool Input::KeyPressed(SDL_Scancode key) {
	return (!prevKeyState[key] && keyState[key]);
}

bool Input::KeyReleased(SDL_Scancode key) {
	return (prevKeyState[key] && !keyState[key]);
}


//M

bool Input::MouseHeld(unsigned char button) {
	return SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button);
}

bool Input::MousePressed(unsigned char button) {
	return !lastMouseButton[button] && (SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
}

bool Input::MouseReleased(unsigned char button) {
	return lastMouseButton[button] && !(SDL_GetMouseState(NULL, NULL) & SDL_BUTTON(button));
}

void Input::SetCursorPos(int x, int y) {
	SDL_WarpMouseInWindow(Graphics::window, x * Graphics::SCREEN_SCALE, y * Graphics::SCREEN_SCALE);
}