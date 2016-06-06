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


List ships;

// Läs in kameran
Camera cam = {
	{0, 2, 3}, 
	{M_PI / 2, 0, 0}
};

void G_InitLevel() {
}

void G_Tick() {
	SYS_UpdateInput();

	// Rörelse relativt till kameran
	vec3 relV = {0};
	if (SYS_keys[IN_RIGHT]) relV[0] += SYS_dSec;
	if (SYS_keys[IN_LEFT]) relV[0] -= SYS_dSec;
	if (SYS_keys[IN_UP]) relV[2] -= SYS_dSec;
	if (SYS_keys[IN_DOWN]) relV[2] += SYS_dSec;

	cam.rot[0] += SYS_var[IN_ROT_X] * SYS_dSec;
	cam.rot[1] -= SYS_var[IN_ROT_Y] * SYS_dSec;

	// TRIGONOMETRI
	double cosinus = cos(cam.rot[1]);
	double sinus = sin(cam.rot[1]);
	cam.pos[2] += relV[2] * cosinus - relV[0] * sinus;
	cam.pos[0] += relV[2] * sinus + relV[0] * cosinus;

	if (SYS_keys[IN_QUIT]) SYS_Quit();
}

