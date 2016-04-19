AUTOMAKE_OPTIONS = foreign

AM_CFLAGS = --pedantic -Wall -std=c99 -I./include/ -I./src/ `sdl2-config --cflags` -DGLEW_STATIC 
AM_LDFLAGS = -L./libs/ `sdl2-config --libs` -lGLEW -lGL -lGLU -lassimp

bin_PROGRAMS = Space
Space_SOURCES = ./src/v_opengl.c ./src/def.c ./src/g_main.c ./src/v_main.c ./src/sys_main.c ./src/sys_input.c ./src/v_opengl.h ./src/g_main.h ./src/v_main.h ./src/sys_main.h ./src/sys_input.h ./src/def.h ./include/linmath/linmath.h
Spacedir = $(datadir)
dist_Space_DATA = ./res ./res/Ship/Ship.dae

V=0
