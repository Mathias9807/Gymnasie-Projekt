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
#include "g_main.h"


double V_fov = 45, V_near = 0.1, V_far = 10;

Model* ship;

void LoadResources() {
	ship = V_LoadModel("res/Ship/Ship.dae");
}

void V_Init() {
	SYS_OpenWindow();
	V_StartOpenGL();
	LoadResources();

	V_SetDepthTesting(true);
	V_MakeProjection();
}

void V_Tick() {
	V_ClearColor(0, 0, 0.4, 0);
	V_ClearDepth();

	V_PushState();
	V_ApplyCamera();
	
	V_RenderModel(ship);
	V_PopState();
}

