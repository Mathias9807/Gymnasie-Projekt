/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "def.h"
#include "sys_input.h"

#include <SDL2/SDL.h>
#include <limits.h>

#define SYS_DEADZONE 8000


// Vilken typ av "kontroll" som används
enum {
	SYS_KEYBOARD,
	SYS_CONTROLLER
};
int SYS_inputDevice = SYS_KEYBOARD;

SDL_GameController* SYS_controller = NULL;

bool SYS_keys[IN_LAST];
float SYS_var[IN_LAST];

void SYS_InitInput() {
	if (SDL_NumJoysticks() > 0) {
		SYS_inputDevice = SYS_CONTROLLER;
		SYS_controller = SDL_GameControllerOpen(0);
	}
}

// Uppdaterar listorna med knappar och spakar
void SYS_UpdateInput() {
	switch (SYS_inputDevice) {
		case SYS_KEYBOARD: {
			const Uint8* state = SDL_GetKeyboardState(NULL);
			
			SYS_var[IN_UP] = state[SDL_SCANCODE_W] 
				- state[SDL_SCANCODE_S];
			SYS_var[IN_RIGHT] = state[SDL_SCANCODE_D]
			       - state[SDL_SCANCODE_A];

			SYS_keys[IN_QUIT]	= state[SDL_SCANCODE_Q];

			int y = 0;
			y += state[SDL_SCANCODE_RIGHT];
			y -= state[SDL_SCANCODE_LEFT];
			SYS_var[IN_ROT_Y] = y;

			int x = 0;
			x -= state[SDL_SCANCODE_UP];
			x += state[SDL_SCANCODE_DOWN];
			SYS_var[IN_ROT_X] = x;
			break;
		}

		case SYS_CONTROLLER: {
			SDL_GameControllerUpdate();

			double u = -SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_LEFTY);
			if (abs(u) <= SYS_DEADZONE) u = 0;
			SYS_var[IN_UP] = u / (double) SHRT_MAX;

			double r = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_LEFTX);
			if (abs(r) <= SYS_DEADZONE) r = 0;
			SYS_var[IN_RIGHT] = r / (double) SHRT_MAX;

			double y = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_RIGHTX);
			if (abs(y) <= SYS_DEADZONE) y = 0;
			SYS_var[IN_ROT_Y] = y / (double) SHRT_MAX;

			double x = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_RIGHTY);
			if (abs(x) <= SYS_DEADZONE) x = 0;
			SYS_var[IN_ROT_X] = x / (double) SHRT_MAX;

			break;
		}
	}
}

