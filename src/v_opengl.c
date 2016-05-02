/*
 * Ett tre-dimensionellt rymd spel
 * 
 * Copyright (c) 2016 by Mathias Johansson
 * 
 * This code is licensed under the MIT license 
 * (https://opensource.org/licenses/MIT)
 */

// För att ladda modell-filer
#include <assimp/cimport.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "sys_main.h"
#include "v_main.h"
#include "v_opengl.h"
#include "g_main.h"


void V_StartOpenGL() {
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) 
		SYS_Error("GLEW failed to initialize");
	
	glGetError(); // Rensar listan med felmeddelanden

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
	printf("OpenGL implementation provided by %s\n", glGetString(GL_VENDOR));
}

void V_WindowResized() {
	V_MakeProjection();

	glViewport(0, 0, SYS_GetWidth(), SYS_GetHeight());
}

void V_ClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void V_ClearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void V_MakeProjection() {
	double aspect = (double) SYS_GetWidth() / SYS_GetHeight();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(V_fov, aspect, V_near, V_far);
	glMatrixMode(GL_MODELVIEW);
}

// Flytta scenen som om den sågs igenom kameran
void V_ApplyCamera() {
	glMatrixMode(GL_MODELVIEW);
	glRotated(cam.rot[0], 1, 0, 0);
	glRotated(cam.rot[2], 0, 0, 1);
	glRotated(cam.rot[1], 0, 1, 0);
	glTranslated(-cam.pos[0], -cam.pos[1], -cam.pos[2]);
}

// Spara alla förflyttningar
void V_PushState() {
	glPushMatrix();
}

// Återställ alla förflyttningar sedan förra V_PushState
void V_PopState() {
	glPopMatrix();
}

Model* V_LoadModel(const char* path) {
	Model* m = malloc(sizeof(Model));
	m->list = glGenLists(1);

	// Hämta modellens absoluta adress
	char fullPath[PATH_LENGTH] = {0};
	strcat(fullPath, SYS_GetBasePath());
	strcat(fullPath, path);
	
	const struct aiScene* scene = aiImportFile(fullPath, 
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_FlipUVs);

	// Skicka meshen till grafikkortet på programstart
	glNewList(m->list, GL_COMPILE);
	glBegin(GL_TRIANGLES);
	// För varje mesh
	for (int i = 0; i < scene->mNumMeshes; i++) {
		struct aiMesh* mesh = scene->mMeshes[i];

		// För varje triangel i meshen
		for (int j = 0; j < mesh->mNumFaces; j++) {
			struct aiFace face = mesh->mFaces[j];
			
			// För varje punkt i triangeln
			for (int k = 0; k < face.mNumIndices; k++) {
				struct aiVector3D vec = mesh->mVertices[face.mIndices[k]];
				struct aiVector3D nrm = mesh->mNormals[face.mIndices[k]];
				struct aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];

				float* color;
				struct aiColor4D diffuse;
				if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)
					== AI_SUCCESS)
					color = (vec4) {diffuse.r, diffuse.g, diffuse.b, diffuse.a};

				// Skicka normalen till grafikkortet
				glNormal3d(nrm.x, nrm.z, nrm.y);

				// Skicka färgen till grafikkortet
				glColor3d(color[0], color[1], color[2]);

				// Skicka punkten till grafikkortet
				glVertex3d(vec.x, vec.z, vec.y);
			}
		}
	}
	glEnd();
	glEndList();

	return m;
}

void V_RenderModel(Model* m) {
	glCallList(m->list);
}

void V_SetDepthTesting(bool b) {
	if (b)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

