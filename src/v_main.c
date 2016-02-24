/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include <GL/glew.h>

#include "sys_main.h"
#include "v_main.h"


void V_StartOpenGL() {
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) 
		SYS_Error("GLEW failed to initialize");
	
	glGetError(); // Rensar listan med errors
}

void V_LoadResources() {
}

