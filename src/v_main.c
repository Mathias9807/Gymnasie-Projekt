/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "sys_main.h"
#include "v_opengl.h"
#include "v_main.h"


Model* ship;

void LoadResources() {
	ship = V_LoadModel("res/Ship/Ship.dae");
}

void V_Init() {
	SYS_OpenWindow();
	V_StartOpenGL();
	LoadResources();

	V_SetDepthTesting(true);
}

void V_Tick() {
	V_ClearColor(0, 0, 0.4, 0);
	V_ClearDepth();

	glPushMatrix();
	glRotated(SYS_GetTime() * 24, 1, 0, 0);
	glRotated(SYS_GetTime() * 24, 0, 1, 0);
	V_RenderModel(ship);
	glPopMatrix();
}

