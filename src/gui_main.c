/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "gui_main.h"
#include "g_main.h"
#include "sys_input.h"
#include "sys_main.h"
#include "v_main.h"
#include "v_opengl.h"


Menu* currentMenu = NULL;

// Modellen som används för att rita ut rektanglar
extern Model* unitPlane;

extern int abstractBox;

Menu* CreateMainMenu();

void GUI_Init() {
	currentMenu = CreateMainMenu();
}

void GUI_Tick() {
}

// Rita menyn
void GUI_Render() {
	if (!currentMenu) return;

	V_SetDepthTesting(false);
	V_SetShader(guiShader);
	ListEntry* le = currentMenu->comps.first;
	do {
		GUI_RenderComp(le->value);
	}while (le->next && (le = le->next));
	V_SetShader(shader);
}

// Rita en komponent
void GUI_RenderComp(MenuComp* comp) {
	switch (comp->type) {
	case Box:
		V_SetParam2f("pos", 0.1, 0.1);
		V_SetParam2f("size", 0.1, 0.1);
		V_BindTexture(abstractBox, 0);
		V_RenderModel(unitPlane);
		break;
	}
}

Menu* CreateMainMenu() {
	Menu* m		= calloc(1, sizeof(Menu));
	MenuComp* c	= calloc(1, sizeof(MenuComp));
	c->type		= Box;
	ListAdd(&m->comps, c);

	return m;
}

