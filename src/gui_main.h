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
	Box, Label, Selector, Radar
} MenuCompType;

// En komponent i en meny
typedef struct MenuComp {
	MenuCompType	type;		// Komponentens typ
	vec2		pos;		// Position på skärmen (från 0 till 1)
	vec2		size;
	float		parallax;	// Hur mycket komp. skiftas med kameran

	void		(*tick)();

	bool		selectable;	// Om det går att markera komponenten
	void		(*action)();	// Rutinen för när komponenten blir klickad

	union { // Variabler som är specifika för komp. typen
		struct {
			vec3 color;
		} box;
		struct {
			struct MenuComp* selected;
		} selector;
		struct {
			bool centered;	// Om texten ska centreras
			char text[256];	// Max 256 bokstäver
			float scale;	// Påverkar storleken av texten
		} label;
		struct {
		} radar;
	} data;
} MenuComp;

// En meny
// Representeras av en lista av komponenter
typedef struct {
	List		comps;
	bool		focusGrabbed;	// Om menyerna tar tangentbordsfokuset
} Menu;

extern Menu*		GUI_currentMenu;
extern bool		GUI_focusGrabbed;
extern Menu		GUI_mainMenu, GUI_inGameMenu, GUI_respawnMenu;


void GUI_Init();
void GUI_Tick();
void GUI_Render();
void GUI_RenderComp(MenuComp* comp);
void GUI_ChangeMenu(Menu* m);
void GUI_CompAddAction(MenuComp* c, void (*action)());

void GUI_OpenInGameMenu();
void GUI_OpenMainMenu();
void GUI_OpenRespawnMenu();
void GUI_Play();

MenuComp* GUI_CreateSelector(Menu* m);
MenuComp* GUI_CreateLabel(Menu* m, char* text, vec2 pos, float size, bool centered);
MenuComp* GUI_CreateRadar(Menu* m);
void GUI_SelectorTick(MenuComp* self);
void GUI_RadarTick(MenuComp* self);


#endif // GUI_MAIN_H
