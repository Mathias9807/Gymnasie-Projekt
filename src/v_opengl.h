/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

#ifndef V_OPENGL_H
#define V_OPENGL_H


#include "v_main.h"
#include <GL/glew.h>
#include <GL/glu.h>

typedef struct {
	GLuint list;
} Model;

extern GLuint curShader;
extern float V_vertFov;

Model* V_LoadModel(const char* path);
void V_StartOpenGL();
void V_RenderModel(Model* m);
void V_ApplyCamera();
void V_PushState();
void V_PopState();
void V_ClearColor(float r, float g, float b, float a);
void V_ClearDepth();
void V_SetDepthTesting(bool b);
void V_SetDepthWriting(bool b);
void V_SetAlphaBlending(bool b);
void V_BindTexture(unsigned id, int pos);
void V_BindCubeMap(unsigned id, int pos);
void V_SetTexRepeating(bool b);
void V_SetTexInterLinear(bool b);
void V_SetTexMipmapLinear(bool b);
void V_MakeProjection();
void V_SetFaceCullingBack(bool back);

GLuint V_LoadShader(char* name);
void V_DeleteShader(int programId);
GLuint V_LoadTexture(char* name);
GLuint V_LoadCubeMap(char* name);
void V_SetAnimMatrix(Model* m, unsigned int time, mat4x4 r);
void V_SetShader(GLuint id);
void V_SetParam1f(const char* var, float f);
void V_SetParam2f(const char* var, float x, float y);
void V_SetParam3f(const char* var, float x, float y, float z);
void V_SetParam4f(const char* var, float x, float y, float z, float a);
void V_SetParam1i(const char* var, int i);
void V_SetParam4m(const char* var, mat4x4 mat);
void V_PrintMat(mat4x4 mat);


#endif // V_OPENGL_H
