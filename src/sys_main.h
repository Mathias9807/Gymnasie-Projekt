/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef SYS_MAIN_H
#define SYS_MAIN_H


#include "def.h"

// Skillnad i tid mellan denna bild och förra bilden
extern double SYS_dSec;

void SYS_Error(char* s);
void SYS_Warning(char* s);
void SYS_OpenWindow();


#endif // SYS_MAIN_H
