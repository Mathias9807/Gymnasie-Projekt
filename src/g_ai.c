/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "g_ai.h"
#include "gui_main.h"
#include "sys_input.h"
#include "sys_main.h"
#include "v_main.h"


void G_ShipHit(Ship* s, Bullet* b) {
	if (SYS_GetTime() > s->shieldTime + G_SHIP_INVULN_TIME)
		s->health--;

	// Hämta vektorn från skölden till impact stället
	vec3 sphere;
	vec3_sub(sphere, b->pos, s->pos);

	// Spara vektorn där skeppet träffades
	vec4 corrected;
	mat4x4 inv;
	mat4x4_invert(inv, s->rot); // Ta bort rotationen
	mat4x4_mul_vec4(corrected, inv, sphere);
	memcpy(s->shieldHit, corrected, sizeof(vec3));
	s->shieldTime = SYS_GetTime();

	// Beräkna vel-vektorns avstånd längs med vektorn
	float d = -vec3_mul_inner(b->vel, sphere);

	// Om skottet redan är på väg ifrån skölden ska inget göras
	float dir = d / abs(d);
	float sDir = vec3_len(sphere);
	sDir = sDir / abs(sDir);
	if (dir == sDir) return;

	// Addera det två gånger till skottets hastighet
	vec3_scale(sphere, sphere, d);
	vec3_add(b->vel, b->vel, sphere);
	vec3_add(b->vel, b->vel, sphere);

	// Sakta ner skottet när det studsar
	vec3_scale(b->vel, b->vel, 0.3);

	// Om skölden är nere absorberas skottet
	if (s->health <= 1.0) G_DeleteBullet(b);
}

void G_PlayerTick(Ship* s) {
	G_ShipExhaust(s, (vec3) {-0.18, 0, 1}, &s->ex0);
	G_ShipExhaust(s, (vec3) { 0.18, 0, 1}, &s->ex1);

	if (GUI_currentMenu && GUI_currentMenu->focusGrabbed) return;
	if (s->health < 1) return;

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
	
	G_RotateShip(G_player, vert * SYS_dSec, horiz * SYS_dSec, tilt * SYS_dSec);

	static bool atkHeld = false;
	if (SYS_keys[IN_ATTACK] && !atkHeld) {
		vec4 v = {0, 0, -50, 0}, r;
		mat4x4_mul_vec4(r, G_player->rot, v);
		G_AddBullet(s, 3, s->pos, r, 0.3);
	}
	atkHeld = SYS_keys[IN_ATTACK];
}

void G_AiBasicTick(Ship* s) {
	G_ShipExhaust(s, (vec3) {-0.18, 0, 1}, &s->ex0);
	G_ShipExhaust(s, (vec3) { 0.18, 0, 1}, &s->ex1);

	if (s->target && s->target->health > 0) {
		Ship* t = s->target;

		// if (SYS_GetTime() - s->lastShot > 1) {
		// 	s->lastShot = SYS_GetTime();
		// 	vec4 diff, n;
		// 	vec4_sub(diff, t->pos, s->pos);
		// 	vec4_norm(n, diff);
		// 	vec4_scale(diff, n, 50);
		// 	G_AddBullet(s, 2, s->pos, diff, 0.3);
		// }
		// G_RotateShip(s, 0, 0, 1 * SYS_dSec);
		// vec4 right = (vec4) {1, 0, 0, 1}, r;
		// mat4x4_mul_vec4(r, s->rot, right);
		// vec4 
		vec4 p, uP, rP;
		vec4_sub(p, t->pos, s->pos);
		vec4_norm(uP, p);
		mat4x4 inv; mat4x4_invert(inv, s->rot);
		mat4x4_mul_vec4(rP, inv, uP);
		float rotSpeed = 0.5, prec = 0.2;
		if (rP[0] > prec) G_RotateShip(s, 0, rotSpeed * SYS_dSec, 0);
		if (rP[0] < -prec) G_RotateShip(s, 0, -rotSpeed * SYS_dSec, 0);
		if (rP[1] > prec) G_RotateShip(s, rotSpeed * SYS_dSec, 0, 0);
		if (rP[1] < -prec) G_RotateShip(s, -rotSpeed * SYS_dSec, 0, 0);

		if (rP[2] < 0 && abs(rP[0]) < 0.1 && abs(rP[1]) < 0.1) {
			if (SYS_GetTime() - s->lastShot > 1) {
				s->lastShot = SYS_GetTime();
				vec4 forwards = {0, 0, -1, 1}, n;
				mat4x4_mul_vec4(n, s->rot, forwards);
				vec4_norm(forwards, n);
				vec4_scale(n, forwards, 300);
				G_AddBullet(s, 3, s->pos, n, 0.3);
			}
		}
	}else {
		double closest = 10000000.0;
		Ship* nTarget = G_ships.first->value;
		for (int i = 0; i < G_ships.size; i++) {
			Ship* ss = ListGet(&G_ships, i);
			if (ss == s || ss->health < 1) continue;
			double c = (s->pos[0]-ss->pos[0])*(s->pos[0]-ss->pos[0])
				+ (s->pos[1]-ss->pos[1])*(s->pos[1]-ss->pos[1])
				+ (s->pos[2]-ss->pos[2])*(s->pos[2]-ss->pos[2]);
			if (c < closest) {
				closest = c;
				nTarget = ss;
			}
		}
		s->target = nTarget;
	}
}

void G_OnDying(Ship* s) {
	G_RotateShip(s, -1 * SYS_dSec, 1 * SYS_dSec, 3 * SYS_dSec);

	// Skjut ut eld partiklar
	if (s->flameTimer == 0) s->flameTimer = SYS_GetTime();
	double freq = 30;
	if (SYS_GetTime() - s->flameTimer > 1 / freq) {
		double speed = 4;
		G_AddParticle(0, s->pos, (vec4) {
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1), 0
		}, 1.5, 0.3);

		s->flameTimer += 1.0 / freq;
	}
}

void G_Die(Ship* s) {
	// Explodera
	for (int i = 0; i < 16; i++) {
		double speed = 6;
		G_AddParticle(1, s->pos, (vec4) {
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1), 0
		}, 3, 0.5);
	}
	for (int i = 0; i < 16; i++) {
		double speed = 6;
		G_AddParticle(0, s->pos, (vec4) {
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1),
			speed * ((double) rand() / RAND_MAX * 2 - 1), 0
		}, 2, 0.5);
	}
}

