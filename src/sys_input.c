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


int SYS_inputDevice = SYS_KEYBOARD;

SDL_GameController* SYS_controller = NULL;

bool SYS_keys[IN_LAST];
float SYS_var[IN_LAST];

void SYS_InitInput() {
	if (SDL_NumJoysticks() > 0) {
		SYS_controller = SDL_GameControllerOpen(0);

		printf("Detected controller\n");
	}
}

// Uppdaterar listorna med knappar och spakar
void SYS_UpdateInput() {
	switch (SYS_inputDevice) {
		case SYS_KEYBOARD: {
			const Uint8* state = SDL_GetKeyboardState(NULL);
			
			SYS_var[IN_LVERT] = state[SDL_SCANCODE_W] 
				- state[SDL_SCANCODE_S];
			SYS_var[IN_LHORIZ] = state[SDL_SCANCODE_D]
			       - state[SDL_SCANCODE_A];
			SYS_var[IN_RVERT] = state[SDL_SCANCODE_UP] 
				- state[SDL_SCANCODE_DOWN];
			SYS_var[IN_RHORIZ] = state[SDL_SCANCODE_RIGHT]
			       - state[SDL_SCANCODE_LEFT];
			
			SYS_var[IN_BOOST] = state[SDL_SCANCODE_LSHIFT];
			SYS_keys[IN_ATTACK] = state[SDL_SCANCODE_SPACE];
			SYS_keys[IN_ACTION] = state[SDL_SCANCODE_E];
			SYS_keys[IN_START] = state[SDL_SCANCODE_RETURN];
			SYS_keys[IN_QUIT] = state[SDL_SCANCODE_Q];
			break;
		}

		case SYS_CONTROLLER: {
			SDL_GameControllerUpdate();
			
			double d = 0;
			d = -SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_LEFTY);
			if (abs(d) <= SYS_DEADZONE) d = 0;
			SYS_var[IN_LVERT] = d / (double) SHRT_MAX;

			d = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_LEFTX);
			if (abs(d) <= SYS_DEADZONE) d = 0;
			SYS_var[IN_LHORIZ] = d / (double) SHRT_MAX;
			
			d = -SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_RIGHTY);
			if (abs(d) <= SYS_DEADZONE) d = 0;
			SYS_var[IN_RVERT] = d / (double) SHRT_MAX;

			d = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_RIGHTX);
			if (abs(d) <= SYS_DEADZONE) d = 0;
			SYS_var[IN_RHORIZ] = d / (double) SHRT_MAX;

			d = SDL_GameControllerGetAxis(SYS_controller, 
					SDL_CONTROLLER_AXIS_TRIGGERLEFT);
			if (abs(d) <= SYS_DEADZONE) d = 0;
			SYS_var[IN_BOOST] = d / (double) SHRT_MAX;

			SYS_keys[IN_ATTACK] = SDL_GameControllerGetAxis(
					SYS_controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT) > SYS_DEADZONE;

			SYS_keys[IN_ACTION] = SDL_GameControllerGetButton(
					SYS_controller, SDL_CONTROLLER_BUTTON_A);

			SYS_keys[IN_START] = SDL_GameControllerGetButton(
					SYS_controller, SDL_CONTROLLER_BUTTON_START);

			SYS_keys[IN_QUIT] = SDL_GameControllerGetButton(
					SYS_controller, SDL_CONTROLLER_BUTTON_BACK);

			break;
		}
	}
}

