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
#include "g_level.h"
#include "sys_input.h"
#include "sys_main.h"
#include "v_main.h"
#include "v_opengl.h"


Menu mainMenu = {0}, inGameMenu = {0}, respawnMenu = {0};
Menu* GUI_currentMenu = NULL;

// Modellen som används för att rita ut rektanglar
extern Model* unitPlane;
int dot, redDot;

extern int abstractBox;

void GUI_OpenInGameMenu() {
	GUI_ChangeMenu(&inGameMenu);
}
void GUI_OpenMainMenu() {
	G_SetupDemoLevel();
	GUI_ChangeMenu(&mainMenu);
}
void GUI_Play() {
	G_SetupFFA(true, 12);
	GUI_ChangeMenu(&inGameMenu);
}
void GUI_OpenRespawnMenu() {
	GUI_ChangeMenu(&respawnMenu);
}
void SpawnPlayer() {
	G_player = G_AddShip((vec3) {0, 5, 50}, NULL, NULL, &G_playerAi);
	G_player->onDeath = GUI_OpenRespawnMenu;
	V_SetCameraFocus(G_player);
	GUI_ChangeMenu(&inGameMenu);
}

void GUI_Init() {
	dot = V_LoadTexture("res/Dot.png");
	redDot = V_LoadTexture("res/Red dot.png");

	// Skapa huvud menyn
	{
		mainMenu.focusGrabbed = true;
		MenuComp* ply, *ctr, *stp;
		GUI_CreateLabel(&mainMenu, "Space game",
				(vec2) {0.5, 0.1}, 0.06, true);

		ply = GUI_CreateLabel(&mainMenu,
				"Play", (vec2) {0.5, 0.4}, 0.04, true);
		ctr = GUI_CreateLabel(&mainMenu,
				"Controls", (vec2) {0.5, 0.5}, 0.04, true);
		stp = GUI_CreateLabel(&mainMenu,
				"Quit", (vec2) {0.5, 0.6}, 0.04, true);

		GUI_CompAddAction(ply, GUI_Play);
		GUI_CompAddAction(ctr, NULL);
		GUI_CompAddAction(stp, SYS_Quit);

		GUI_CreateSelector(&mainMenu);
	}

	// Skapa menyn som visas under gameplay
	{
		inGameMenu.focusGrabbed = false;

		GUI_CreateRadar(&inGameMenu);
	}

	// Skapa respawn menyn
	{
		respawnMenu.focusGrabbed = false;

		MenuComp* ply, *quit;

		GUI_CreateLabel(&respawnMenu, "You Died!",
				(vec2) {0.5, 0.2}, 0.07, true);

		ply = GUI_CreateLabel(&respawnMenu,
				"Respawn", (vec2) {0.5, 0.45}, 0.05, true);
		quit = GUI_CreateLabel(&respawnMenu,
				"Quit", (vec2) {0.5, 0.6}, 0.05, true);

		GUI_CompAddAction(ply, SpawnPlayer);
		GUI_CompAddAction(quit, SYS_Quit);

		GUI_CreateSelector(&respawnMenu);
	}
}

// Uppdatera komponenterna
void GUI_Tick() {
	ListEntry* le = GUI_currentMenu->comps.first;
	while (le) {
		MenuComp* c = le->value;
		if (c->tick) c->tick(c);

		le = le->next;
	}
}

// Rita menyn
void GUI_Render() {
	if (!GUI_currentMenu) return;

	V_SetDepthTesting(false);
	V_SetShader(guiShader);
	ListEntry* le = GUI_currentMenu->comps.first;
	while (le) {
		GUI_RenderComp(le->value);

		le = le->next;
	}
}

// Rita en komponent
void GUI_RenderComp(MenuComp* comp) {
	V_SetParam2f("pos", comp->pos[0] + parallaxShift[0] * comp->parallax,
			comp->pos[1] - parallaxShift[1] * comp->parallax);
	V_SetParam2f("size", comp->size[0], comp->size[1]);

	switch (comp->type) {
	case Label: {
		float scale = comp->data.label.scale;
		vec2 textSize = {
			(float) V_FONT_CHAR_WIDTH * strlen(comp->data.label.text)
				/ V_FONT_CHAR_SIZE * scale, scale
		};

		V_RenderText(comp->data.label.text, (vec2) {
				comp->pos[0] - comp->data.label.centered
				* textSize[0] / 2,
				comp->pos[1] - comp->data.label.centered
				* textSize[1] / 2}, scale);

		break;
	}case Selector: {
		float scale = 0.05;
		MenuComp* t = comp->data.selector.selected;
		if (!t) break;

		V_RenderText("[", (vec2) {
				t->pos[0] - t->size[0] / 2 -
				scale * V_FONT_CHAR_WIDTH / V_FONT_CHAR_SIZE,
				t->pos[1] - scale / 2}, scale);
		V_RenderText("]", (vec2) {
				t->pos[0] + t->size[0] / 2 + scale * 4.0 /
				V_FONT_CHAR_SIZE, t->pos[1] - scale / 2}, scale);

		break;
	}case Box: {
		V_SetParam2f("subPos", 0, 0);
		V_SetParam2f("subSize", 1, 1);
		V_BindTexture(abstractBox, 0);
		V_RenderModel(unitPlane);

		break;
	}case Radar: {
		V_SetParam2f("subPos", 0, 0);
		V_SetParam2f("subSize", 1, 1);
		vec2 p = {0.1, 0.8};
		float scl = 0.1;
		float ratio = (float) SYS_GetWidth() / SYS_GetHeight();
		V_SetShader(guiShader);
		V_BindTexture(dot, 0);
		V_ApplyCamera();
		for (int i = 0; i < G_ships.size; i++) {
			Ship* s = ListGet(&G_ships, i);
			if (G_player == s || s->health <= 0) continue;

			vec4 d = {0, 0, 0, 1}, r = {0, 0, 0, 1}, P;
			memcpy(P, s->pos, sizeof(vec3)); P[3] = 1.0;

			// World offset between player and ship
			vec3_sub(d, P, G_player->pos);

			// Rotate offset to player space
			mat4x4 inv;
			mat4x4_invert(inv, G_player->rot);
			mat4x4_mul_vec4(r, inv, d);
			vec3_norm(d, r);
			if (d[2] > 0) V_BindTexture(redDot, 0);
			else V_BindTexture(dot, 0);

			float localSize = 0.02 / (1 + abs(d[2] / 8));
			V_SetParam2f("pos", p[0] + d[0]*scl, p[1] - d[1]*scl);
			V_SetParam2f("size", localSize, ratio * localSize);
			V_RenderModel(unitPlane);
		}
		V_BindTexture(dot, 0);
		V_SetParam2f("pos", p[0] - 0.005, p[1] - 0.1);
		V_SetParam2f("size", 0.01, 0.2);
		V_RenderModel(unitPlane);

		V_SetParam2f("pos", p[0] - 0.1, p[1] - 0.005);
		V_SetParam2f("size", 0.2, 0.01);
		V_RenderModel(unitPlane);

		break;
	}
	}
}

void GUI_ChangeMenu(Menu* m) {
	GUI_currentMenu = m;
}

void GUI_CompAddAction(MenuComp* c, void (*action)()) {
	c->selectable = true;
	c->action = action;
}

