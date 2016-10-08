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
	IN_LVERT, IN_LHORIZ, IN_RVERT, IN_RHORIZ, IN_ATTACK, IN_BOOST, IN_QUIT, IN_LAST
};

extern bool SYS_keys[IN_LAST];
extern float SYS_var[IN_LAST];

// Vilken typ av "kontroll" som används
enum {
	SYS_KEYBOARD,
	SYS_CONTROLLER
};
extern int SYS_inputDevice;

void SYS_InitInput();
void SYS_UpdateInput();


#endif // SYS_INPUT_H
