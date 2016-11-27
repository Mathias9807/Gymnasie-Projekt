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

Menu* currentMenu = NULL;


void GUI_Tick() {
}

// Rita menyn
void GUI_Render() {
	if (!currentMenu) return;

	for (int i = 0; i < currentMenu->comps.size; i++) {
		GUI_RenderComp(ListGet(&currentMenu->comps, i));
	}
}

// Rita en komponent
void GUI_RenderComp(MenuComp* comp) {
	switch (comp->type) {
	case Box:
		break;
	}
}

