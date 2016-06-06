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
Camera* V_camera = &cam;

Model* ship, * cube;

int tex;

void LoadResources() {
	ship = V_LoadModel("res/Sphere.dae");
	cube = V_LoadModel("res/cube.dae");

	tex = V_LoadTexture("res/Cubemap 1/Cubemap.png");
}

void V_Init() {
	SYS_OpenWindow();
	V_StartOpenGL();
	LoadResources();

	V_SetParam1i("diffuse_tex", 0);

	V_SetDepthTesting(true);

	mat4x4_identity(V_projMat);
	mat4x4_identity(V_worldMat);
	mat4x4_identity(V_modelMat);
	V_MakeProjection();

	// glEnable(GL_COLOR_MATERIAL);
	// glEnable(GL_LIGHTING);
	// glEnable(GL_LIGHT0);
	// glShadeModel(GL_SMOOTH);

	// glLightfv(GL_LIGHT0, GL_AMBIENT, (float[4]) {0.3, 0.3, 0.3, 1});
	// glLightfv(GL_LIGHT0, GL_DIFFUSE, (float[4]) {0.5, 0.5, 0.5, 1});
	// glLightfv(GL_LIGHT0, GL_POSITION, (float[4]) {0.0, 1, 0, 1});
	// glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, 4);
}

void V_Tick() {
	V_ClearColor(0, 0, 0.4, 0);
	V_ClearDepth();

	V_PushState();
	V_ApplyCamera();
	
	for (int i = 0; i < 3; i++)
		V_worldMat[3][i] = 0;

	V_SetDepthWriting(false);
	V_BindTexture(tex, 0);
	V_UseTextures(true);
	V_RenderModel(cube);
	V_SetDepthWriting(true);
	V_PopState();

	V_PushState();
	V_ApplyCamera();

	V_UseTextures(false);
	
	mat4x4_rotate_Z(V_modelMat, V_modelMat, SYS_GetTime() * 2);
	//mat4x4_rotate_Z(V_modelMat, V_modelMat, M_PI / 2);

	V_RenderModel(ship);

	V_PopState();

	SYS_CheckErrors();
}

