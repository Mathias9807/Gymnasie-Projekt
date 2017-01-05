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

vec2 parallaxShift;

Model* ship, * cube;
Model* plane;
Model* unitPlane;
Model* billboard;

// Alla partikel texturer
#define NUM_PART_TEXTURES 8
int partTextures[NUM_PART_TEXTURES];
int fontTex;

int tex, schack, abstractBox;
int crosshair1, crosshair2;

void LoadResources() {
	ship = V_LoadModel("res/Ship/Ship.dae");
	cube = V_LoadModel("res/cube.dae");
	plane = V_LoadModel("res/Plane.dae");
	unitPlane = V_LoadModel("res/unit-plane.dae");
	billboard = V_LoadModel("res/billboard.dae");

	tex = V_LoadTexture("res/Cubemap 1/Cubemap.png");
	schack = V_LoadTexture("res/Schack.png");
	abstractBox = V_LoadTexture("res/Abstract Box.png");
	crosshair1 = V_LoadTexture("res/square-crosshair.png");
	crosshair2 = V_LoadTexture("res/diamond-crosshair.png");

	fontTex = V_LoadTexture("res/ubuntu-mono.png");

	int i = 0;
	partTextures[i++] = V_LoadTexture("res/Smoke.png");
	partTextures[i++] = V_LoadTexture("res/Flame.png");
	partTextures[i++] = V_LoadTexture("res/Sphere.png");
}

void V_Init() {
	SYS_OpenWindow();
	V_StartOpenGL();
	LoadResources();

	V_SetShader(guiShader);
	V_SetParam1i("tex", 0);
	V_SetParam2f("subPos", 0, 0);
	V_SetParam2f("subSize", 1, 1);
	V_SetShader(shader);
	V_SetParam1i("diffuse_tex", 0);

	V_SetDepthTesting(true);

	mat4x4_identity(V_projMat);
	mat4x4_identity(V_worldMat);
	mat4x4_identity(V_modelMat);
}

int particleComparator(void* a, void* b) {
	vec3 posA;
	vec3 posB;
	vec3_sub(posA, ((Particle*)a)->pos, camera->pos);
	vec3_sub(posB, ((Particle*)b)->pos, camera->pos);
	
	if ((posA[0] * posA[0] + posA[1] * posA[1] + posA[2] * posA[2])
			> (posB[0] * posB[0] + posB[1] * posB[1] + posB[2] * posB[2]))
		return 1;
	if ((posA[0] * posA[0] + posA[1] * posA[1] + posA[2] * posA[2])
			< (posB[0] * posB[0] + posB[1] * posB[1] + posB[2] * posB[2]))
		return -1;
	
	return 0;
}

void V_Tick() {
	// Uppdatera parallax vektorn
	if (camera && camera->focus) {
		Ship* focus = camera->focus;
		Ship* ghost = camera->ghost;

		// Applicera rotations marticen och gångra sen med inversen
		// av spökets rotationsmatris för att få skillnaden
		vec4 r, v = {0, 0, -1, 1};
		mat4x4_mul_vec4(r, focus->rot, v);

		mat4x4 inv;
		mat4x4_invert(inv, ghost->rot);

		memcpy(v, r, sizeof(vec4));
		mat4x4_mul_vec4(r, inv, v);

		memcpy(parallaxShift, r, sizeof(vec2));
	}else {
		parallaxShift[0] = 0.0;
		parallaxShift[1] = 0.0;
	}

	mat4x4 tmp;

	mat4x4_identity(V_modelMat);
	mat4x4_identity(V_worldMat);
	V_ApplyCamera();

	V_SetDepthWriting(true);
	V_SetDepthTesting(true);

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
		mat4x4_translate_in_place(V_modelMat,
			s->pOffs[0], s->pOffs[1], s->pOffs[2]);
		mat4x4_rotate_Z(V_modelMat, V_modelMat, s->zOffs);

		V_RenderModel(ship);
	}
	
	// Sortera partiklarna efter avstånd
	ListBubbleSort(&G_particles, particleComparator);

	// Rita partiklarna
	V_UseTextures(false);
	V_IsParticle(true);
	V_SetDepthWriting(false);
	V_PushState();
	for (int i = 0; i < G_particles.size; i++) {
		Particle* p = ListGet(&G_particles, i);
		V_BindTexture(partTextures[p->texture], 0);

		mat4x4_translate(V_modelMat, p->pos[0], p->pos[1], p->pos[2]);
		mat4x4_scale(V_modelMat, V_modelMat, 1 / p->scale);

		V_RenderModel(billboard);
	}
	V_PopState();
	V_IsParticle(false);
	V_SetDepthWriting(false);
	V_SetDepthTesting(false);

	V_PopState();
	V_UseTextures(true);

	// Rita det närmre siktet
	mat4x4_translate(V_modelMat,
		G_player->pos[0], G_player->pos[1], G_player->pos[2]);
	mat4x4_mul(V_modelMat, V_modelMat, G_player->rot);
	mat4x4_translate_in_place(V_modelMat, 0, 0, -20);
	mat4x4_rotate_X(V_modelMat, V_modelMat, M_PI / 2);

	V_BindTexture(crosshair1, 0);
	V_RenderModel(unitPlane);

	// Rita siktet längre bort
	mat4x4_translate(V_modelMat,
		G_player->pos[0], G_player->pos[1], G_player->pos[2]);
	mat4x4_mul(V_modelMat, V_modelMat, G_player->rot);
	mat4x4_translate_in_place(V_modelMat, 0, 0, -60);
	mat4x4_rotate_X(V_modelMat, V_modelMat, M_PI / 2);
	mat4x4_scale_aniso(V_modelMat, V_modelMat, 2, 2, 2);

	V_BindTexture(crosshair2, 0);
	V_RenderModel(unitPlane);

	// Kolla så att allt gick bra
	SYS_CheckErrors();
}

void V_RenderText(const char* text, vec2 pos, float scale) {
	V_SetParam2f("size", scale, scale);
	for (int i = 0; text[i]; i++) {
		char c = text[i];

		V_SetParam2f("pos", pos[0] + i * scale
				* V_FONT_CHAR_WIDTH / 16, pos[1]);
		V_SetParam2f("subPos", (c % 32) * 16.0 / V_FONT_SIZE,
			(c / 32) * 16.0 / V_FONT_SIZE);
		V_SetParam2f("subSize", 1.0 / 32, 1.0 / 32);
		V_BindTexture(fontTex, 0);
		V_RenderModel(unitPlane);
	}
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

