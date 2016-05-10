/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef V_MAIN_H
#define V_MAIN_H


#define V_WIN_WIDTH 800
#define V_WIN_HEIGHT 600


#include "def.h"
#include "g_main.h"

extern double V_fov, V_near, V_far;
extern Camera* V_camera;

void V_Init();
void V_Tick();
void V_WindowResized();


#endif // V_MAIN_H
