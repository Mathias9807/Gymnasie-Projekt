/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#include "sys_main.h"
#include "v_main.h"
#include "v_opengl.h"


void V_StartOpenGL() {
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) 
		SYS_Error("GLEW failed to initialize");
	
	glGetError(); // Rensar listan med errors

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("OpenGL implementation provided by %s\n", glGetString(GL_VENDOR));
}

