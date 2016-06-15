/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef G_MAIN_H
#define G_MAIN_H


#include "def.h"

typedef struct {
	vec3 pos, rot;
} Ship;
extern List G_ships;
Ship* G_player;

typedef struct {
	// Absoluta koordinater
	vec3 pos;
	vec3 rot;

	// Alternativt relativa koordinater
	// Om focus == 0 används absoluta koordinater
	Ship* focus;
	vec3 pOffs;
	vec2 rOffs;

	double fov, near, far;
} Camera;
extern Camera cam;

void G_InitLevel();
void G_Tick();


#endif // G_MAIN_H
