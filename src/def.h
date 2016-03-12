/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef DEF_H
#define DEF_H


#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <linmath/linmath.h>
#include "math.h"

#define TITLE "Space"
#define VERSION "0.1"

#define PATH_LENGTH 256

typedef struct listEntryStruct {
	void* value;
	struct listEntryStruct* next;
	struct listEntryStruct* prev;
} listEntry;

typedef struct {
	listEntry* first;
	int size;
} list;

typedef struct {
	int w;			// Dimensions of pixel array
	int h;
	uint32_t* pix;	// Pixel array in format ARGB
} sprite;

void* ListGet(list* l, int index);
int ListFind(list* l, void* value);
int ListSize(list* l);
void ListAdd(list* l, void* value);
void ListRemove(list* l, int index);


#endif // DEF_H
