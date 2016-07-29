/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef S_MAIN_H
#define S_MAIN_H


#include "def.h"
#include "sys_main.h"

struct _Clip;
typedef struct _Clip Clip;

void S_Init();
void S_Quit();
Clip S_LoadWav(const char* link);


#endif // S_MAIN_H
