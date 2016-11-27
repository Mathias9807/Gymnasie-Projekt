/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef GUI_MAIN_H
#define GUI_MAIN_H


#include "def.h"


// Alla komponent typer
typedef enum {
	Box
} MenuCompType;

// En komponent i en meny
typedef struct {
	MenuCompType	type;	// Komponentens typ
	vec2		pos;	// Position på skärmen (från 0 till 1)
	vec2		size;

	union { // Variabler som är specifika för komp. typen
		struct {
			vec3 color;
		} box;
	} data;
} MenuComp;

// En meny
// Representeras av en lista av komponenter
typedef struct {
	List		comps;
} Menu;

extern Menu*		currentMenu;


void GUI_Tick();
void GUI_Render();
void GUI_RenderComp(MenuComp* comp);


#endif // GUI_MAIN_H
