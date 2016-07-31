/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "s_main.h"
#include "v_main.h"

#include <al.h>
#include <alc.h>


struct _Clip {
	int bitsPerSample;
	int sampleFreq;
	int channels;

	long long size;
	char* data;

	ALuint bufferID;
};
List S_sources;

ALCdevice* device;

Clip rockets;

uint16_t getInt16(unsigned char* data);
uint32_t getInt32(unsigned char* data);

void S_Init() {
	// Starta OpenAL
	device = alcOpenDevice(NULL);
	if (device == NULL) {
		SYS_Error("Failed to open an audio device!\n");
	}

	ALCcontext* context = alcCreateContext(device, NULL);
	if (context == NULL) {
		SYS_Error("Failed to initialize OpenAL!\n");
	}
	alcMakeContextCurrent(context);
	
	rockets = S_LoadWav("res/Rockets.wav");

	ListAdd(&S_sources, S_CreateSource());

	AudioSource* shipSource = S_CreateSource();
	shipSource->s = ListGet(&G_ships, 1);
	ListAdd(&S_sources, shipSource);
}

void S_Tick() {
	// Riktningar och positioner
	vec4 up;
	vec4 forward;
	vec4 pos;

	mat4x4 inverted = {0};
	mat4x4_invert(inverted, V_worldMat);

	mat4x4_mul_vec4(up,		inverted, (vec4) {0, 1, 0, 0});
	mat4x4_mul_vec4(forward,	inverted, (vec4) {0, 0, -1, 0});
	mat4x4_mul_vec4(pos,		inverted, (vec4) {0, 0, 0, 1});

	float orientation[6];
	memcpy(orientation, forward, sizeof(vec3));
	memcpy(&orientation[3], up, sizeof(vec3));
	alListenerfv(AL_ORIENTATION, orientation);

	alListenerfv(AL_POSITION, pos);

	ListEntry* e = S_sources.first;
	for (int i = 0; i < S_sources.size; i++, e = e->next) {
		AudioSource* s = e->value;

		float* p = s->pos;
		float* v = s->vel;

		if (s->s != NULL) {
			p = s->s->pos;
			v = s->s->vel;
		}

		if (s->c != NULL) {
			p = s->c->pos;
			memset(v, 0, sizeof(vec3));
		}

		alSourcefv(s->sourceID, AL_POSITION, p);
		alSourcefv(s->sourceID, AL_VELOCITY, v);
	}

	static bool ayy = true;
	if (ayy) {
		ayy = false;

		S_PlayClip(&rockets, ListGet(&S_sources, 1), true);
	}
}

AudioSource* S_CreateSource() {
	AudioSource* s = calloc(1, sizeof(AudioSource));

	alGenSources(1, &s->sourceID);
	alSourcef(s->sourceID, AL_PITCH, 1);
	alSourcef(s->sourceID, AL_GAIN, S_GLOBAL_VOLUME / 100.0);
	alSource3f(s->sourceID, AL_POSITION, 0, 0, 0);
	alSource3f(s->sourceID, AL_VELOCITY, 0, 0, 0);

	return s;
}

void S_PlayClip(Clip* c, AudioSource* source, bool repeating) {
	alSourcei(source->sourceID, AL_BUFFER, rockets.bufferID);
	alSourcei(source->sourceID, AL_LOOPING, repeating ? AL_TRUE : AL_FALSE);
	alSourcePlay(source->sourceID);
}

void S_Quit() {
	alcMakeContextCurrent(NULL);
	alcCloseDevice(device);
}

// Ladda en wav ljud-fil
Clip S_LoadWav(const char* link) {
	// Öppna filen
	char path[PATH_LENGTH] = {0};
	strcpy(path, SYS_GetBasePath());
	strcpy(path, "/");
	strcpy(path, link);

	FILE* file = fopen(path, "rb");
	if (!file) {
		SYS_Error("Couldn't open file! \n");
		return (Clip) {0};
	}

	Clip c;
	
	// Läs headern
	unsigned char riff[12];
	for (int i = 0; i < 12; i++)
		riff[i] = fgetc(file);

	// Kolla att filen verkligen är en .wav fil
	if (memcmp(riff, "RIFF", 4) != 0) {
		SYS_Error("Sound file is not a .wav file\n");
		fclose(file);
		return (Clip) {0};
	}

	if (memcmp(&riff[8], "WAVE", 4) != 0) {
		SYS_Error("Sound file is not a .wav file\n");
		fclose(file);
		return (Clip) {0};
	}

	unsigned char fmt[24];
	for (int i = 0; i < 24; i++)
		fmt[i] = fgetc(file);

	// Läs formatet
	c.channels = getInt16(&fmt[10]);
	c.bitsPerSample = getInt16(&fmt[22]);
	c.sampleFreq = getInt32(&fmt[12]);

	unsigned char dataHeader[8];
	for (int i = 0; i < 8; i++)
		dataHeader[i] = fgetc(file);

	c.size = getInt32(&dataHeader[4]);

	// Läs datan
	c.data = calloc(1, c.size);
	for (int i = 0; i < c.size; i++)
		c.data[i] = fgetc(file);

	// Skicka datan till OpenAL
	alGenBuffers(1, &c.bufferID);
	alBufferData(c.bufferID,
		c.bitsPerSample == 8
			? (c.channels == 1 ? AL_FORMAT_MONO8 : AL_FORMAT_STEREO8)
			: (c.channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16),
		c.data, c.size, c.sampleFreq);

	fclose(file);
	return c;
}

uint16_t getInt16(unsigned char* data) {
	uint16_t i = data[0];
	i |= data[1] << 8;
	return i;
}

uint32_t getInt32(unsigned char* data) {
	uint32_t i = data[0];
	i |= data[1] << 8;
	i |= data[2] << 16;
	i |= data[3] << 24;
	return i;
}

