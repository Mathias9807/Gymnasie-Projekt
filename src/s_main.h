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

#define S_GLOBAL_VOLUME 50

struct Clip;
typedef struct Clip Clip;
struct AudioSource;
typedef struct AudioSource AudioSource;

#include "g_main.h"

struct Clip {
	int bitsPerSample;
	int sampleFreq;
	int channels;

	long long size;
	char* data;

	unsigned bufferID;
};

struct AudioSource {
	unsigned sourceID;

	Ship* s;

	Camera* c;

	vec3 pos, vel;
};
extern List S_sources;

extern Clip S_rockets;

void S_Init();
void S_Tick();
void S_Quit();

AudioSource* S_CreateSource(Ship* s);
void S_DeleteSource(AudioSource* source);
void S_PlayClip(Clip* c, AudioSource* source, bool repeating);
Clip S_LoadWav(const char* link);

AudioSource* S_CreateSource();


#endif // S_MAIN_H
