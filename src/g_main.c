/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "g_main.h"
#include "gui_main.h"
#include "sys_input.h"
#include "sys_main.h"
#include "v_main.h"


List G_ships;
Ship* G_player;
List G_particles;
List G_bullets;

Ai playerAi, stupidAi;

// Läs in kameran
Camera cam = {
	.pOffs = {0, 2, 4}, .rOffs = {-M_PI / 16, 0},
	.fov = 45, .near = 0.1, .far = 300
};

void RotateShip(Ship* s, float x, float y, float z);
void playerTick(Ship* s);
void aiBasicTick(Ship* s);

// Ökningen i hastighet när ett skepp boostar
double acceleration(double t) {
	return 3 * t * t - 2 * t * t * t;
}

void G_InitLevel() {
	playerAi = (Ai) {NULL, playerTick, NULL};
	stupidAi = (Ai) {NULL, aiBasicTick, NULL};

	G_player = G_AddShip(NULL, NULL, NULL, &playerAi);

	G_AddShip((vec3) {1, 1, -50}, NULL, NULL, &stupidAi);

	Particle* p = calloc(1, sizeof(Particle));
	memcpy(p->pos, (vec3) {20, 0, 0}, sizeof(vec3));
	p->scale = 1;
	ListAdd(&G_particles, p);
	
	p = calloc(1, sizeof(Particle));
	memcpy(p->pos, (vec3) {20, 5, 10}, sizeof(vec3));
	p->texture = 2;
	p->scale = 5;
	ListAdd(&G_particles, p);
	
	p = calloc(1, sizeof(Particle));
	memcpy(p->pos, (vec3) {25, 5, 10}, sizeof(vec3));
	p->texture = 2;
	p->scale = 5;
	ListAdd(&G_particles, p);
	
	p = calloc(1, sizeof(Particle));
	memcpy(p->pos, (vec3) {22.5f, 5, 15}, sizeof(vec3));
	p->texture = 2;
	p->scale = 5;
	ListAdd(&G_particles, p);
	
	V_SetCamera(&cam);
	V_SetCameraFocus(G_player);
}

void G_Tick() {
	SYS_UpdateInput();

	// Stäng ner programmet vid begäran
	if (SYS_keys[IN_QUIT]) SYS_running = false;

	// Kör de individuella skeppens logik
	for (int i = 0; i < G_ships.size; i++) {
		Ship* s = ListGet(&G_ships, i);

		if (s->ai && s->ai->live)
			s->ai->live(s);
	}

	// Applicera hastigheten och accelerationen på alla skepp
	for (int i = 0; i < G_ships.size; i++) {
		Ship* s = ListGet(&G_ships, i);

		vec4 v = {0, 0, -1, 0}, r;
		mat4x4_mul_vec4(r, s->rot, v);
		float speed = s->baseSpeed + s->accSpeed 
			* acceleration(s->accT);
		s->vel[0] = speed * r[0];
		s->vel[1] = speed * r[1];
		s->vel[2] = speed * r[2];

		for (int i = 0; i < 3; i++)
			s->pos[i] += s->vel[i] * SYS_dSec;
	}

	// Applicera hastigheten till alla partiklar
	for (int i = 0; i < G_particles.size; i++) {
		Particle* p = ListGet(&G_particles, i);
		
		if (p->spawnTime && SYS_GetTime() - p->spawnTime > p->lifeTime) {
			ListRemove(&G_particles, i);
			i--;
			continue;
		}
		
		for (int j = 0; j < 3; j++)
			p->pos[j] += p->vel[j] * SYS_dSec;
	}

	// Ta bort gamla skott
	for (int i = 0; i < G_bullets.size; i++) {
		Bullet* b = ListGet(&G_bullets, i);
		
		if (b->spawnTime && SYS_GetTime() - b->spawnTime > b->lifeTime) {
			ListRemove(&G_particles, ListFind(&G_particles, b->p));
			ListRemove(&G_bullets, i);
			
			i--;
			continue;
		}
	}

	// Flytta kameran till skeppet i fokus
	if (cam.ghost) {
		Ship* g = cam.ghost;
		Ship* f = cam.focus;

		// Hur mycket kameran ska flyttas
		// Halveringstidsformeln används (N = N_0 * ½^(t / T_½)
		double tP = pow(1.0 / 2, SYS_dSec / V_CAM_AVST_HALV_TID);
		double tR = pow(1.0 / 2, SYS_dSec / V_CAM_ROT_HALV_TID);

		for (int i = 0; i < 3; i++) 
			g->pos[i] = interp(f->pos[i], g->pos[i], tP);

		for (int i = 0; i < 3; i++) 
			g->vel[i] = interp(f->vel[i], g->vel[i], tP);

		// Det här är naivt och korkat, man kan inte interpolera
		// rotationsmatriser genom att bara interpolera alla skalärer.
		//
		// Men ändå så funkar det
		for (int i = 0; i < 4; i++) 
			for (int j = 0; j < 4; j++) 
				g->rot[i][j] = interp(f->rot[i][j], g->rot[i][j], tR);
	}
}

Ship* G_AddShip(vec3 p, vec3 v, mat4x4 r, Ai* ai) {
	Ship* s = calloc(1, sizeof(Ship));

	if (p) memcpy(s->pos, p, sizeof(vec3));

	if (v) memcpy(s->vel, v, sizeof(vec3));

	mat4x4_identity(s->rot);
	if (r) memcpy(s->rot, r, sizeof(mat4x4));

	s->accTFactor	= 1.2;
	s->accSpeed	= 16;
	s->baseSpeed	= 8;

	if (ai) s->ai = ai;

	ListAdd(&G_ships, s);

	if (ai->spawn) s->ai->spawn(s);

	return s;
}

void playerTick(Ship* s) {
	if (GUI_currentMenu && GUI_currentMenu->focusGrabbed) return;

	// Beräkna den nya accelerationen
	float boost = 0;
	boost += SYS_dSec * (SYS_var[IN_BOOST] * 2 - 1);
	G_player->accT += boost * G_player->accTFactor;
	G_player->accT = min(G_player->accT, 1);
	G_player->accT = max(G_player->accT, 0);

	// Hämta rotationen relativt till skeppets nuvarande orientation
	double vert = 0, horiz = 0, tilt = 0;
	vert += SYS_var[IN_RVERT] - SYS_var[IN_LVERT];
	horiz += SYS_var[IN_RHORIZ];
	tilt += SYS_var[IN_LHORIZ];
	
	RotateShip(G_player, vert * SYS_dSec, horiz * SYS_dSec, tilt * SYS_dSec);

	static bool atkHeld = false;
	if (SYS_keys[IN_ATTACK] && !atkHeld) {
		Bullet* b = calloc(1, sizeof(Bullet));
		b->p = calloc(1, sizeof(Particle));
		b->p->texture = 2;
		b->p->scale = 0.3;
		b->lifeTime = 1;
		b->spawnTime = SYS_GetTime();
		memcpy(b->p->pos, G_player->pos, sizeof(vec3));
		vec4 v = {0, 0, -50, 0}, r;
		mat4x4_mul_vec4(r, G_player->rot, v);
		memcpy(b->p->vel, r, sizeof(vec3));
		ListAdd(&G_bullets, b);
		ListAdd(&G_particles, b->p);
	}
	atkHeld = SYS_keys[IN_ATTACK];
}

void aiBasicTick(Ship* s) {
	RotateShip(s, 0, 0, 1 * SYS_dSec);
}

// Roterar ett skepp (x = pitch, y = yaw, z = roll)
void RotateShip(Ship* s, float x, float y, float z) {
	vec4 zAxis, yAxis, xAxis;
	mat4x4 rot;
	mat4x4_identity(rot);
	mat4x4_mul_vec4(zAxis, s->rot, (vec4) {0, 0, 1, 0});
	mat4x4_mul_vec4(yAxis, s->rot, (vec4) {0, 1, 0, 0});
	mat4x4_mul_vec4(xAxis, s->rot, (vec4) {1, 0, 0, 0});
	mat4x4_rotate(rot, rot, xAxis[0], xAxis[1], xAxis[2], 
		x);
	mat4x4_rotate(rot, rot, yAxis[0], yAxis[1], yAxis[2], 
		-y);
	mat4x4_rotate(rot, rot, zAxis[0], zAxis[1], zAxis[2], 
		-z);
	mat4x4_mul(s->rot, rot, s->rot);
}

