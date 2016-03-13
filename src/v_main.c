/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "sys_main.h"
#include "v_opengl.h"
#include "v_main.h"


void LoadResources() {
}

void V_Init() {
	SYS_OpenWindow();
	V_StartOpenGL();
	LoadResources();
}

void V_Tick() {
	
}

