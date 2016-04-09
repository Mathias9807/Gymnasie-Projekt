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
	vec3 pos;
} Ship;
extern List ships;

typedef struct {
	vec3 pos;
	vec3 rot;
} Camera;
extern Camera cam;

void G_InitLevel();
void G_Tick();


#endif // G_MAIN_H
