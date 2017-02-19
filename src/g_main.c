/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "g_main.h"
#include "g_ai.h"
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

// Ökningen i hastighet när ett skepp boostar
double acceleration(double t) {
	return 3 * t * t - 2 * t * t * t;
}

void G_InitLevel() {
	playerAi = (Ai) {NULL, G_PlayerTick, G_Die, G_ShipHit, G_OnDying};
	stupidAi = (Ai) {NULL, G_AiBasicTick, G_Die, G_ShipHit, G_OnDying};

	G_player = G_AddShip(NULL, NULL, NULL, &playerAi);

	G_AddShip((vec3) {1, 9, -50}, NULL, NULL, &stupidAi);
	G_AddShip((vec3) {9, 5, -50}, NULL, NULL, &stupidAi);
	G_AddShip((vec3) {-5, 9, -60}, NULL, NULL, &stupidAi);

	G_AddParticle(0, (vec3) {20, 0, 0}, NULL, 1, 0);
	
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

		if (s->health <= 0) {
			if (s->deadTime == 0) s->deadTime = SYS_GetTime();

			if (s->ai && s->ai->onDying)
				s->ai->onDying(s);
		}
	}

	// Applicera hastigheten till alla partiklar
	for (int i = 0; i < G_particles.size; i++) {
		Particle* p = ListGet(&G_particles, i);

		if (p->deltaScale != 0.0) p->scale *= pow(p->deltaScale, SYS_dSec);
		
		if (p->spawnTime && SYS_GetTime() - p->spawnTime > p->lifeTime) {
			ListRemove(&G_particles, i);
			i--;
			continue;
		}
		
		for (int j = 0; j < 3; j++)
			p->pos[j] += p->vel[j] * SYS_dSec;
	}

	// Applicera hastigheten till alla skott
	for (int i = 0; i < G_bullets.size; i++) {
		Bullet* b = ListGet(&G_bullets, i);

		if (b->deltaScale != 0.0) b->scale *= pow(b->deltaScale, SYS_dSec);
		
		// Ta bort gamla skott
		if (b->spawnTime && SYS_GetTime() - b->spawnTime > b->lifeTime) {
			ListRemove(&G_bullets, i);
			i--;
			continue;
		}
		
		for (int j = 0; j < 3; j++)
			b->pos[j] += b->vel[j] * SYS_dSec;

		// Kolla om något skepp har blivit träffat
		for (int j = 0; j < G_ships.size; j++) {
			Ship* s = ListGet(&G_ships, j);

			// Skott kan inte träffa skepp de sköts från
			if (s == b->s) continue;

			// Kolla först "manhattan" avståndet och sedan det verkliga
			vec3 d = {0};
			vec3_sub(d, s->pos, b->pos);
			float r2 = (b->scale + s->size) * (b->scale + s->size);
			if (d[0]*d[0] 
					+ d[1]*d[1] 
					+ d[2]*d[2] < r2) {
				if (s->ai && s->ai->onHit)
					s->ai->onHit(s, b);
			}
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
	s->size = 1.44;

	mat4x4_identity(s->rot);
	if (r) memcpy(s->rot, r, sizeof(mat4x4));

	s->accTFactor	= 1.2;
	s->accSpeed	= 16;
	s->baseSpeed	= 12;
	s->maxHealth = 5;
	s->health = s->maxHealth;

	ListAdd(&G_ships, s);

	if (ai) s->ai = ai;
	if (ai->spawn) s->ai->spawn(s);

	s->source = S_CreateSource(s);
	S_PlayClip(&S_rockets, s->source, true);

	return s;
}

void G_DeleteShip(Ship* s) {
	S_DeleteSource(s->source);
	ListRemove(&G_ships, ListFind(&G_ships, s));
}

Particle* G_AddParticle(int tex, vec3 pos, vec3 vel, float scale, double life) {
	Particle* p = calloc(1, sizeof(Particle));

	p->texture = tex;
	if (pos) memcpy(p->pos, pos, sizeof(vec3));
	if (vel) memcpy(p->vel, vel, sizeof(vec3));
	p->scale = scale;
	p->lifeTime = life;
	p->spawnTime = SYS_GetTime();

	return ListAdd(&G_particles, p);
}

Bullet* G_AddBullet(Ship* s, int tex, vec3 pos, vec3 vel, float scale) {
	Bullet* b = calloc(1, sizeof(Bullet));

	b->s = s;
	b->texture = tex;
	if (pos) memcpy(b->pos, pos, sizeof(vec3));
	if (vel) memcpy(b->vel, vel, sizeof(vec3));
	b->scale = scale;
	b->lifeTime = 10;
	b->spawnTime = SYS_GetTime();

	return ListAdd(&G_bullets, b);
}

void G_DeleteBullet(Bullet* b) {
	ListRemove(&G_bullets, ListFind(&G_bullets, b));
}

// Roterar ett skepp (x = pitch, y = yaw, z = roll)
void G_RotateShip(Ship* s, float x, float y, float z) {
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

// Skapar eld partiklar bakom skepp (lastExhaust: när förra partikeln skapades)
void G_ShipExhaust(Ship* s, vec3 offs, double* lastExhaust) {
	if (*lastExhaust == 0) *lastExhaust = SYS_GetTime();
	double now = SYS_GetTime();
	const int freq = 30;
	while (now - *lastExhaust > 1.0 / freq) {
		vec4 pos = {0, 0, 0, 1};
		mat4x4_mul_vec4(pos, s->rot, offs);
		vec4_add(pos, pos, s->pos);
		{
			vec4_add(pos, pos, (vec4) {
				(rand() % 100 - 50) / 2000.0,
				(rand() % 100 - 50) / 2000.0,
				(rand() % 100 - 50) / 2000.0, 0});
		}
		Particle* p = G_AddParticle(1, pos, NULL, 0.3, 0.1);
		p->deltaScale = 0.0001;
		*lastExhaust += 1.0 / freq;
	}
}

