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

SDL_Window* window;

void SYS_Error(char* s) {
	printf("A fatal error occured: %s\n", s);
	exit(-1);
}

void SYS_Warning(char* s) {
	printf("A non-fatal error occured: %s\n", s);
}

void OpenWindow() {
	// Starta SDL
	SDL_Init(SDL_INIT_VIDEO);

	// Öppna ett fönster
	window = SDL_CreateWindow(
		"Space", 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		V_WIN_WIDTH, V_WIN_HEIGHT, SDL_WINDOW_OPENGL
	);

	// Starta OpenGL
	SDL_GL_CreateContext(window);
}

int main(int argc, char* argv[]) {
	OpenWindow();
	V_StartOpenGL();
	V_LoadResources();

	while (!SDL_QuitRequested()) {

	}

	return 0;
}

