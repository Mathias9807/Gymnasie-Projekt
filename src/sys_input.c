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


bool SYS_keys[IN_LAST];
float SYS_var[IN_LAST];

// Uppdaterar listorna med knappar och spakar
void SYS_UpdateInput() {
	const Uint8* state = SDL_GetKeyboardState(NULL);
	
	SYS_keys[IN_UP] = state[SDL_SCANCODE_W];
	SYS_keys[IN_DOWN] = state[SDL_SCANCODE_S];
	SYS_keys[IN_RIGHT] = state[SDL_SCANCODE_D];
	SYS_keys[IN_LEFT] = state[SDL_SCANCODE_A];
	SYS_keys[IN_QUIT] = state[SDL_SCANCODE_Q];

	int y = 0;
	y += state[SDL_SCANCODE_RIGHT];
	y -= state[SDL_SCANCODE_LEFT];
	SYS_var[IN_ROT_Y] = y;

	int x = 0;
	x -= state[SDL_SCANCODE_UP];
	x += state[SDL_SCANCODE_DOWN];
	SYS_var[IN_ROT_X] = x;
}

