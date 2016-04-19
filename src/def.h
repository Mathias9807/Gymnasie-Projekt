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
#undef VERSION
#define VERSION "indev"

#define PATH_LENGTH 256

typedef struct ListEntryStruct {
	void* value;
	struct ListEntryStruct* next;
	struct ListEntryStruct* prev;
} ListEntry;

typedef struct {
	ListEntry* first;
	int size;
} List;

typedef struct {
	int w;			// Dimensions of pixel array
	int h;
	uint32_t* pix;	// Pixel array in format ARGB
} sprite;

void* ListGet(List* l, int index);
int ListFind(List* l, void* value);
int ListSize(List* l);
void ListAdd(List* l, void* value);
void ListRemove(List* l, int index);


#endif // DEF_H
