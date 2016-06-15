/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "g_main.h"
#include "sys_input.h"
#include "sys_main.h"
#include "v_main.h"


List G_ships;
Ship* G_player;

// Läs in kameran
Camera cam = {
	.pOffs = {0, 2, 4}, .rOffs = {-M_PI / 16, 0},
	.fov = 45, .near = 0.1, .far = 100
};

// Var kameran ska sitta relativt med skeppet
vec3 camPos = {0, 2, 4};

void G_InitLevel() {
	G_player = calloc(1, sizeof(Ship));
	ListAdd(&G_ships, G_player);
	
	cam.focus = G_player;
	V_SetCamera(&cam);
}

void G_Tick() {
	SYS_UpdateInput();

	// Rörelse relativt till kameran
	vec3 relV = {0};
	if (SYS_keys[IN_RIGHT]) relV[0] += SYS_dSec;
	if (SYS_keys[IN_LEFT]) relV[0] -= SYS_dSec;
	if (SYS_keys[IN_UP]) relV[2] -= SYS_dSec;
	if (SYS_keys[IN_DOWN]) relV[2] += SYS_dSec;

	G_player->rot[0] += SYS_var[IN_ROT_X] * SYS_dSec;
	G_player->rot[1] -= SYS_var[IN_ROT_Y] * SYS_dSec;

	// TRIGONOMETRI
	double cosinus = cos(G_player->rot[1]);
	double sinus = sin(G_player->rot[1]);
	G_player->pos[2] += relV[2] * cosinus - relV[0] * sinus;
	G_player->pos[0] += relV[2] * sinus + relV[0] * cosinus;

	if (SYS_keys[IN_QUIT]) SYS_Quit();
}

