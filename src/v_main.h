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

#define V_CAM_AVST_HALV_TID	0.1
#define V_CAM_ROT_HALV_TID	0.1


#include "def.h"
#include "g_main.h"

extern double V_fov, V_near, V_far;

extern mat4x4 V_projMat, V_worldMat, V_modelMat;

void V_Init();
void V_Tick();
void V_SetCamera(Camera* c);
void V_SetCameraFocus(Ship* s);
void V_ApplyCamera();
void V_WindowResized();


#endif // V_MAIN_H
