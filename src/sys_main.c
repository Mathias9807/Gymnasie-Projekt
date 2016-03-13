/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include "sys_main.h"
#include "v_main.h"
#include "g_main.h"

SDL_Window* window;

double SYS_dSec;

void SYS_Error(char* s) {
	printf("A fatal error occured: %s\n", s);
	exit(-1);
}

void SYS_Warning(char* s) {
	printf("A non-fatal error occured: %s\n", s);
}

void SYS_OpenWindow() {
	// Starta SDL
	if (SDL_Init(SDL_INIT_VIDEO)) {
		SYS_Error("Couldn't start SDL");
	}

	// Öppna ett fönster
	window = SDL_CreateWindow(
		TITLE " - V" VERSION, 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		V_WIN_WIDTH, V_WIN_HEIGHT, SDL_WINDOW_OPENGL
	);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Starta OpenGL
	SDL_GL_CreateContext(window);
}

int main(int argc, char* argv[]) {
	V_Init();
	G_InitLevel();

	double now = SDL_GetTicks() / 1000.0;
	double last = now;
	while (!SDL_QuitRequested()) {
		now = SDL_GetTicks() / 1000.0;
		SYS_dSec = now - last;
		last = now;

		G_Tick();
		V_Tick();
	}

	return 0;
}

