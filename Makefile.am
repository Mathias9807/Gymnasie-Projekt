AUTOMAKE_OPTIONS = foreign

AM_CFLAGS = --pedantic -Wall -std=c99 -isystem ./include/ -I./src/ `sdl2-config --cflags` `pkg-config --cflags openal` -DGLEW_STATIC -g
AM_LDFLAGS = -L./libs/ `sdl2-config --libs` `pkg-config --libs openal` -lGLEW -lGL -lGLU -lassimp -lm

bin_PROGRAMS = Space
Space_SOURCES = ./src/v_opengl.c ./src/def.c ./src/g_main.c ./src/g_level.c ./src/g_ai.c ./src/v_main.c ./src/s_main.c ./src/gui_main.c ./src/gui_components.c ./src/sys_main.c ./src/sys_input.c ./src/v_opengl.h ./src/g_main.h ./src/g_level.h ./src/g_ai.h ./src/v_main.h ./src/s_main.h ./src/gui_main.h ./src/sys_main.h ./src/sys_input.h ./src/def.h ./include/linmath/linmath.h ./include/stb/stb_image.h
Spacedir = $(datadir)
dist_Space_DATA = ./res ./res/Ship/Ship.dae

V=0
