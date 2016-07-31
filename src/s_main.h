/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef S_MAIN_H
#define S_MAIN_H


#include "def.h"
#include "sys_main.h"
#include "g_main.h"

#define S_GLOBAL_VOLUME 50

struct _Clip;
typedef struct _Clip Clip;

typedef struct {
	unsigned sourceID;

	Ship* s;

	Camera* c;

	vec3 pos, vel;
} AudioSource;
extern List S_sources;

void S_Init();
void S_Tick();
void S_Quit();

void S_PlayClip(Clip* c, AudioSource* source, bool repeating);
Clip S_LoadWav(const char* link);

AudioSource* S_CreateSource();


#endif // S_MAIN_H
