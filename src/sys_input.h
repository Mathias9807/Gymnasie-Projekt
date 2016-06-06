/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef SYS_INPUT_H
#define SYS_INPUT_H


#include "def.h"

// Indexen för varje kontroll i SYS_keys och SYS_var
enum {
	IN_UP, IN_DOWN, IN_RIGHT, IN_LEFT, IN_ROT_X, IN_ROT_Y, IN_LAST
};

extern bool SYS_keys[IN_LAST];
extern float SYS_var[IN_LAST];

void SYS_UpdateInput();


#endif // SYS_INPUT_H
