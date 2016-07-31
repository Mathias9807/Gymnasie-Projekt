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


// Kameran som ritar scenen
Camera* camera;

Model* ship, * cube;
Model* plane;

int tex, schack;

void LoadResources() {
	ship = V_LoadModel("res/Ship/Ship.dae");
	cube = V_LoadModel("res/cube.dae");
	plane = V_LoadModel("res/Plane.dae");

	tex = V_LoadTexture("res/Cubemap 1/Cubemap.png");
	schack = V_LoadTexture("res/Schack.png");
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
}

void V_Tick() {
	mat4x4 tmp;

	mat4x4_identity(V_worldMat);
	V_ApplyCamera();

	// Rensa skärmen
	V_ClearColor(0, 0, 0.4, 0);
	V_ClearDepth();

	// Rita bakgrundsbilden genom att rita en kub runtom
	// kameran med depth-writing avstängt
	V_PushState();
	
	// Sätt positionsdatan till 0 så kuben ligger på kameran
	for (int i = 0; i < 3; i++)
		V_worldMat[3][i] = 0;

	// Rita kuben
	V_SetDepthWriting(false);
	V_BindTexture(tex, 0);
	V_UseTextures(true);
	V_RenderModel(cube);
	V_SetDepthWriting(true);
	V_PopState();

	V_PushState();

	// Rita ett schackbräde
	V_PushState();
	mat4x4_identity(tmp);
	mat4x4_translate_in_place(tmp, 0, -2, 0);
	mat4x4_scale_aniso(V_modelMat, tmp, 256, 256, 256);
	V_BindTexture(schack, 0);
	V_RenderModel(plane);
	V_PopState();

	// Rita rymdskeppen
	V_UseTextures(false);
	for (int i = 0; i < G_ships.size; i++) {
		Ship* s = ListGet(&G_ships, i);

		mat4x4_translate(V_modelMat, s->pos[0], s->pos[1], s->pos[2]);
		mat4x4_mul(V_modelMat, V_modelMat, s->rot);

		V_RenderModel(ship);
	}

	V_PopState();

	// Kolla så att allt gick bra
	SYS_CheckErrors();
}

// Sätt kamerans värden
void V_SetCamera(Camera* c) {
	camera = c;

	V_MakeProjection();
}

void V_SetCameraFocus(Ship* s) {
	camera->focus = s;

	Ship* g = calloc(1, sizeof(Ship));
	memcpy(g, s, sizeof(Ship));
	camera->ghost = g;
}

