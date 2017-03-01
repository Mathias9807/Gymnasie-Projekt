/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef G_LEVEL_H
#define G_LEVEL_H


#include "def.h"
#include "g_main.h"


void G_CleanLevel();

void G_SetupDemoLevel();
void G_SetupFFA(bool player, int others);


#endif // G_LEVEL_H
