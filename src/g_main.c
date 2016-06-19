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

// Ökningen i hastighet när ett skepp boostar
double acceleration(double t) {
	return 3 * t * t - 2 * t * t * t;
}

void G_InitLevel() {
	G_player = calloc(1, sizeof(Ship));
	G_player->accTFactor	= 0.4;
	G_player->accSpeed	= 16;
	G_player->baseSpeed	= 8;
	ListAdd(&G_ships, G_player);

	Ship* s = calloc(1, sizeof(Ship));
	s->pos[2] = -10;
	s->rot[1] = M_PI * 3 / 4;
	ListAdd(&G_ships, s);
	
	cam.focus = G_player;
	V_SetCamera(&cam);
}

void G_Tick() {
	SYS_UpdateInput();

	// Rörelse relativt till kameran
	float boost = 0;
	if (SYS_keys[IN_UP])	boost += SYS_dSec;
	if (SYS_keys[IN_DOWN])	boost -= SYS_dSec;
	G_player->accT += boost * G_player->accTFactor;
	G_player->accT = min(G_player->accT, 1);
	G_player->accT = max(G_player->accT, 0);

	// G_player->rot[0] += SYS_var[IN_ROT_X] * SYS_dSec;
	G_player->rot[1] -= SYS_var[IN_ROT_Y] * SYS_dSec;

	// TRIGONOMETRI
	double cosinus = cos(G_player->rot[1]);
	double sinus = sin(G_player->rot[1]);
	// G_player->vel[2] += relV[2] * cosinus - relV[0] * sinus;
	// G_player->vel[0] += relV[2] * sinus + relV[0] * cosinus;
	float speed = G_player->baseSpeed + G_player->accSpeed 
		* acceleration(G_player->accT);
	G_player->vel[2] = -speed * cosinus;
	G_player->vel[0] = -speed * sinus;

	for (int i = 0; i < 3; i++)
		G_player->pos[i] += G_player->vel[i] * SYS_dSec;

	if (SYS_keys[IN_QUIT]) SYS_Quit();
}

