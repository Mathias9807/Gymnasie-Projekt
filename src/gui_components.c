/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "gui_main.h"
#include "sys_main.h"
#include "sys_input.h"
#include "v_main.h"


MenuComp* GUI_CreateLabel(Menu* m, char* text, vec2 pos, float size, bool centered) {
	MenuComp* c	= calloc(1, sizeof(MenuComp));
	c->type		= Label;
	c->parallax	= 0.1;
	c->tick		= NULL;
	memcpy(c->pos, pos, sizeof(vec2));

	int len = strlen(text);
	c->size[0] = size * len * V_FONT_CHAR_WIDTH / V_FONT_CHAR_SIZE;
	c->size[1] = size;

	if (len > 255) {
		SYS_Error("GUI_CreateLabel called with a too long 'text' string");
	}
	strcpy(c->data.label.text, text);
	c->data.label.centered = centered;
	c->data.label.scale = size;

	ListAdd(&m->comps, c);
	return c;
}

MenuComp* GUI_CreateSelector(Menu* m) {
	MenuComp* c	= calloc(1, sizeof(MenuComp));
	c->type		= Selector;
	c->parallax	= 0.1;
	c->tick		= GUI_SelectorTick;

	// Select the first selectable comp in the current menu
	ListEntry* le = m->comps.first;
	while (le) {
		MenuComp* mc = le->value;
		if (mc->selectable) {
			c->data.selector.selected = mc;
			break;
		}

		le = le->next;
	}

	ListAdd(&m->comps, c);
	return c;
}

void GUI_SelectorTick(MenuComp* self) {
	bool up = false, down = false, action = false;

	// Kolla vilka knappar som trycktes ner
	static bool upHeld = false;
	if (SYS_var[IN_LVERT] > 0.5) {
		if (!upHeld) {
			up = true;
			upHeld = true;
		}else up = false;
	}else upHeld = false;

	static bool downHeld = false;
	if (SYS_var[IN_LVERT] < -0.5) {
		if (!downHeld) {
			down = true;
			downHeld = true;
		}else down = false;
	}else downHeld = false;

	static bool actionHeld = false;
	if (SYS_keys[IN_ACTION] || SYS_keys[IN_START]) {
		if (!actionHeld) {
			action = true;
			actionHeld = true;
		}else action = false;
	}else actionHeld = false;

	if (up) {
		int index = ListFind(&GUI_currentMenu->comps,
				self->data.selector.selected);
		if (index == -1) goto skipUp;

		for (int i = index - 1; i >= 0; i--) {
			MenuComp* newTarget = ListGet(&GUI_currentMenu->comps, i);
			if (newTarget->selectable) {
				self->data.selector.selected = newTarget;
				break;
			}
		}
	}
skipUp:

	if (down) {
		int index = ListFind(&GUI_currentMenu->comps,
				self->data.selector.selected);
		if (index == -1) goto skipDown;

		for (int i = index + 1; i < GUI_currentMenu->comps.size; i++) {
			MenuComp* newTarget = ListGet(&GUI_currentMenu->comps, i);
			if (newTarget->selectable) {
				self->data.selector.selected = newTarget;
				break;
			}
		}
	}
skipDown:

	if (action && self->data.selector.selected
			&& self->data.selector.selected->action) {
		self->data.selector.selected->action();
	}
}

