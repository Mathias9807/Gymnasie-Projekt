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
#include "sys_input.h"
#include "v_main.h"
#include "g_main.h"
#include "s_main.h"
#include "gui_main.h"

SDL_Window* window;
extern SDL_GameController* SYS_controller;

// Delta tid mellan frames
double SYS_dSec;

bool SYS_running = true;

// Program argument
int	SYS_argc = 0;
char** SYS_argv = NULL;

void SYS_Error(char* s) {
	printf("A fatal error occurred: %s\n", s);
	exit(-1);
}

void SYS_Warning(char* s) {
	printf("A non-fatal error occurred: %s\n", s);
}

// Hämta adressen av mappen som programmet ligger i
const char* SYS_GetBasePath() {
	static char path[PATH_LENGTH] = {0};

	// Hämta bara adressen om det är första gången funktionen körs
	if (path[0] == '\0') {
		char* sPath = SDL_GetBasePath();
		for (int i = 0; sPath[i]; i++)
			path[i] = sPath[i];

		SDL_free(sPath);
	}

	return path;
}

// Ger tiden sedan programstart i sekunder
double SYS_GetTime() {
	static double start = -1;
	if (start == -1) start = SDL_GetTicks() / 1000.0;

	return SDL_GetTicks() / 1000.0 - start;
}

void SYS_CheckErrors() {
	// Decode OpenGL error codes
	switch (glGetError()) {
		case GL_NO_ERROR: break;
		case GL_INVALID_ENUM: printf("OpenGL Error: GL_INVALID_ENUM\n"); break;
		case GL_INVALID_VALUE: printf("OpenGL Error: GL_INVALID_VALUE\n"); break;
		case GL_INVALID_OPERATION: printf("OpenGL Error: GL_INVALID_OPERATION\n"); break;
		case GL_STACK_OVERFLOW: printf("OpenGL Error: GL_STACK_OVERFLOW\n"); break;
		case GL_STACK_UNDERFLOW: printf("OpenGL Error: GL_STACK_UNDERFLOW\n"); break;
		case GL_OUT_OF_MEMORY: printf("OpenGL Error: GL_OUT_OF_MEMORY\n"); break;
		case GL_INVALID_FRAMEBUFFER_OPERATION: printf("OpenGL Error: GL_INVALID_FRAMEBUFFER_OPERATION\n"); break;
		case GL_TABLE_TOO_LARGE: printf("OpenGL Error: GL_TABLE_TOO_LARGE\n"); break;
	}
}

// Kolla om programmet var startat med ett visst argument
bool SYS_HasParam(char* p) {
	for (int i = 0; i < SYS_argc; i++) {
		if (strcmp(SYS_argv[i], p) == 0) 
			return true;
	}
	return false;
}

void SYS_OpenWindow() {
	// Starta SDL
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER)) {
		SYS_Error("Couldn't start SDL");
	}

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);

	// Öppna ett fönster
	window = SDL_CreateWindow(
		TITLE " - " VERSION, 
		SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 
		V_WIN_WIDTH, V_WIN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE
	);

	// Starta OpenGL
	SDL_GL_CreateContext(window);

	SYS_CheckErrors();
}

// Uppdatera skärmen
void SYS_UpdateWindow() {
	SDL_GL_SwapWindow(window);
}

void SYS_CheckEvents() {
	SDL_Event e;

	while (SDL_PollEvent(&e)) {
		switch (e.type) {
			case SDL_WINDOWEVENT:
				V_WindowResized();
				break;

			// Byter vilka kontroller som används
			case SDL_CONTROLLERBUTTONUP: {
				if (SYS_inputDevice == SYS_CONTROLLER) break;

				memset(SYS_keys, 0, sizeof(SYS_keys));
				memset(SYS_var, 0, sizeof(SYS_var));

				SYS_inputDevice = SYS_CONTROLLER;
				break;
			}

			case SDL_KEYUP: {
				if (SYS_inputDevice == SYS_KEYBOARD) break;

				memset(SYS_keys, 0, sizeof(SYS_keys));
				memset(SYS_var, 0, sizeof(SYS_var));

				SYS_inputDevice = SYS_KEYBOARD;
				break;
			}

			case SDL_CONTROLLERDEVICEADDED: {
				printf("A controller was added!\n");
				
				if (!SYS_controller)
					SYS_controller = SDL_GameControllerOpen(0);
				
				break;
			}

			case SDL_CONTROLLERDEVICEREMOVED: {
				printf("A controller was disconnected\n");
				
				if (SDL_NumJoysticks() > 0) break;
				
				SYS_controller = NULL;
				
				memset(SYS_keys, 0, sizeof(SYS_keys));
				memset(SYS_var, 0, sizeof(SYS_var));

				SYS_inputDevice = SYS_KEYBOARD;
				
				break;
			}
		}
	}
}

int SYS_GetWidth() {
	int w;
	SDL_GetWindowSize(window, &w, NULL);
	return w;
}

int SYS_GetHeight() {
	int h;
	SDL_GetWindowSize(window, NULL, &h);
	return h;
}

void SYS_Quit() {
	SDL_Quit();
	exit(0);
}

// Programmets start punkt
int main(int argc, char* argv[]) {
	SYS_argc = argc;
	SYS_argv = argv;

	if (SYS_HasParam("--version") || SYS_HasParam("-v")) {
		printf("Version: %s\n", VERSION);

		return 0;
	}

	V_Init();

	SYS_InitInput();
	
	G_InitLevel();

	GUI_Init();

	S_Init();

	double now = SDL_GetTicks() / 1000.0;
	double last = now;
	while (!SDL_QuitRequested() && SYS_running) {
		now = SDL_GetTicks() / 1000.0;
		SYS_dSec = now - last;
		last = now;

		SYS_CheckEvents();
		G_Tick();
		GUI_Tick();
		S_Tick();
		V_Tick();
		GUI_Render();
		SYS_UpdateWindow();

		SYS_CheckErrors();
	}

	S_Quit();

	SYS_Quit();
	return 0;
}

