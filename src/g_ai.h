/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef G_AI_H
#define G_AI_H


#include "def.h"
#include "g_main.h"


void G_ShipHit(Ship* s, Bullet* b);
void G_PlayerTick(Ship* s);
void G_AiBasicTick(Ship* s);
void G_OnDying(Ship* s);
void G_Die(Ship* s);


#endif // G_AI_H
