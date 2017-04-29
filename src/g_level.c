/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "g_level.h"
#include "gui_main.h"
#include "v_main.h"


void FocusOnRandomShip() {
	if (G_ships.size == 0) return;

	Ship* s = ListGet(&G_ships, 
			(int) ((double) rand() / RAND_MAX * G_ships.size));
	V_SetCameraFocus(s);
	s->onDeath = FocusOnRandomShip;
}

void G_CleanLevel() {
	// Ta bort alla skepp
	while (G_ships.size > 0) {
		G_DeleteShip(ListGet(&G_ships, 0));
	}
	G_player = NULL;

	// Ta bort alla skott
	for (int i = 0; i < G_bullets.size; i++) {
		G_DeleteBullet(ListGet(&G_bullets, i));
	}

	// Ta bort alla partiklar
	for (int i = 0; i < G_particles.size; i++) {
		G_DeleteParticle(ListGet(&G_particles, i));
	}

	V_SetCameraFocus(NULL);
}

void G_SetupDemoLevel() {
	G_CleanLevel();

	Ship* s = G_AddShip((vec3) {0, 5, 50}, NULL, NULL, &G_stupidAi);
	V_SetCameraFocus(s);
	s->onDeath = FocusOnRandomShip;

	G_AddShip((vec3) {50, 7, 0}, NULL, NULL, &G_stupidAi);
	G_AddShip((vec3) {-50, 2, 0}, NULL, NULL, &G_stupidAi);
}

void G_SetupFFA(bool player, int others) {
	G_CleanLevel();

	if (player) {
		G_player = G_AddShip((vec3) {0, 5, 50}, NULL, NULL, &G_playerAi);
		G_player->onDeath = GUI_OpenRespawnMenu;
		V_SetCameraFocus(G_player);
	}

	for (int i = 0; i < others + player; i++) {
		if (player && i == 0) continue;

		float theta = -M_PI / 2 + (float) i / (others + player) * 2 * M_PI;
		float radius = 100;
		Ship* s = G_AddShip((vec3) {radius*cos(theta), 5,
				-radius*sin(theta)}, NULL, NULL, &G_stupidAi);
		mat4x4_identity(s->rot);
		G_RotateShip(s, 0, -M_PI * 2 * i / (player + others), 0);
	}
}

