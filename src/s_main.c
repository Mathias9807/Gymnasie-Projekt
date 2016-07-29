/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "s_main.h"


struct _Clip {
	int bitsPerSample;
	int sampleFreq;
	int channels;

	long long size;
	char* data;
};

uint16_t getInt16(unsigned char* data);
uint32_t getInt32(unsigned char* data);

void S_Init() {
	S_LoadWav("res/beep.wav"); 
}

// Ladda en wav ljud-fil
Clip S_LoadWav(const char* link) {
	// Öppna filen
	FILE* file = fopen(link, "rb");
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

