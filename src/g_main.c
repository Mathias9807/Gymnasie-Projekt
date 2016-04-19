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
	{30, 0, 0}
};

void G_InitLevel() {
}

void G_Tick() {
	SYS_UpdateInput();

	if (SYS_keys[IN_RIGHT]) cam.pos[0] += SYS_dSec;
	if (SYS_keys[IN_LEFT]) cam.pos[0] -= SYS_dSec;
	if (SYS_keys[IN_UP]) cam.pos[2] -= SYS_dSec;
	if (SYS_keys[IN_DOWN]) cam.pos[2] += SYS_dSec;
}

