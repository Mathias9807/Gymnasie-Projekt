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

// För att ladda bilder
#define STB_IMAGE_IMPLEMENTATION
#include <stb/stb_image.h>

#include "sys_main.h"
#include "v_main.h"
#include "v_opengl.h"
#include "g_main.h"


#ifdef V_GLES_API
GLuint shader = 0;
int vertAttrib = 0, colAttrib = 0;

typedef struct {
	float x, y, z;
	float r, g, b;
} Vertex;
#endif

void V_StartOpenGL() {
#ifdef V_GL_API
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) 
		SYS_Error("GLEW failed to initialize");
#else
	// Ladda shader programmet
	shader = V_LoadShader("shader");
	glUseProgram(shader);

	vertAttrib = glGetAttribLocation(shader, "vertex_in");
	glEnableVertexAttribArray(vertAttrib);
	colAttrib = glGetAttribLocation(shader, "color_in");
	glEnableVertexAttribArray(colAttrib);
#endif
	
	glGetError(); // Rensar listan med felmeddelanden

	printf("OpenGL Version: %s\n", glGetString(GL_VERSION));
#ifdef V_GL_API
	printf("GLSL Version: %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));
#endif
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
#ifdef V_GL_API
	double aspect = (double) SYS_GetWidth() / SYS_GetHeight();

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// gluPerspective(V_fov, aspect, V_near, V_far);
	glOrtho(-1, 1, -1, 1, -1, 1);
	glMatrixMode(GL_MODELVIEW);
#endif // V_GL_API
}

// Flytta scenen som om den sågs igenom kameran
void V_ApplyCamera() {
#ifdef V_GL_API
	glMatrixMode(GL_MODELVIEW);
	glRotated(cam.rot[0], 1, 0, 0);
	glRotated(cam.rot[2], 0, 0, 1);
	glRotated(cam.rot[1], 0, 1, 0);
	glTranslated(-V_camera->pos[0], -V_camera->pos[1], -V_camera->pos[2]);
#endif // V_GL_API
}

// Spara alla förflyttningar
void V_PushState() {
#ifdef V_GL_API
	glPushMatrix();
#endif // V_GL_API
}

// Återställ alla förflyttningar sedan förra V_PushState
void V_PopState() {
#ifdef V_GL_API
	glPopMatrix();
#endif // V_GL_API
}

Model* V_LoadModel(const char* path) {
	Model* m = calloc(1, sizeof(Model));
#ifdef V_GL_API
	m->list = glGenLists(1);
#else
	glGenBuffers(1, &m->vertId);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertId);
#endif

	// Hämta modellens absoluta adress
	char fullPath[PATH_LENGTH] = {0};
	strcat(fullPath, SYS_GetBasePath());
	strcat(fullPath, path);
	
	const struct aiScene* scene = aiImportFile(fullPath, 
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_FlipUVs);

	// Skicka meshen till grafikkortet på programstart
#ifdef V_GL_API
	glNewList(m->list, GL_COMPILE);
	glBegin(GL_TRIANGLES);
#else
	// Räkna hur många verticeer som finns
	m->numVerts = 0;
	for (int i = 0; i < scene->mNumMeshes; i++) {
		for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) 
			m->numVerts += scene->mMeshes[i]->mFaces[j].mNumIndices;
	}

	Vertex* vertBuffer = calloc(1, sizeof(Vertex) * m->numVerts);

	// Håller koll på vilka indexer som redan är fyllda i vertBuffer
	int curVert = 0;
#endif

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
				struct aiVector3D uv = {0, 0, 0};
				struct aiMaterial* mtl = scene->mMaterials[mesh->mMaterialIndex];

				float* color;
				struct aiColor4D diffuse;
				if (aiGetMaterialColor(mtl, AI_MATKEY_COLOR_DIFFUSE, &diffuse)
					== AI_SUCCESS)
					color = (vec4) {diffuse.r, diffuse.g, diffuse.b, diffuse.a};

				if (mesh->mNumUVComponents[0] > 0) {
					uv = mesh->mTextureCoords[0][face.mIndices[k]];
					// Skicka UV koordinaten
#ifdef V_GL_API
					glTexCoord2d(uv.x, uv.y);
#endif
				}

#ifdef V_GL_API
				// Skicka normalen till grafikkortet
				glNormal3d(nrm.x, nrm.z, nrm.y);

				// Skicka färgen
				glColor3d(color[0], color[1], color[2]);

				// Skicka punkten
				glVertex3d(vec.x, vec.z, vec.y);
#else
				vertBuffer[curVert++] = (Vertex) {
					vec.x, vec.z, vec.y,
					color[0], color[1], color[2]
				};
#endif
			}
		}
	}
#ifdef V_GL_API
	glEnd();
	glEndList();
#else
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m->numVerts, 
		vertBuffer, GL_STATIC_DRAW);

	free(vertBuffer);
#endif

	return m;
}

GLuint V_LoadTexture(char* name) {
	char path[PATH_LENGTH] = {0};
	strcpy(path, SYS_GetBasePath());
	strcpy(path, "/");
	strcpy(path, name);
	unsigned char* data;
	int w, h, n;
	data = stbi_load(path, &w, &h, &n, 4);
	if (data == NULL) 
		SYS_Warning("Couldn't load texture");
	
	GLuint tex;
	glGenTextures(1, &tex);
	glBindTexture(GL_TEXTURE_2D, tex);
#ifdef V_GL_API
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
#else
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
#endif
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
#ifdef V_GL_API
	glGenerateMipmap(GL_TEXTURE_2D);
#endif
	
	stbi_image_free(data);
	return tex;
}

void V_CheckShader(GLuint id) {
	GLint result = -1, logLength = -1;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_TRUE) return;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
	GLchar log[logLength];
	glGetShaderInfoLog(id, logLength, &logLength, log);
	printf("%s\n", log);
}

void V_CheckProgram(GLuint id) {
	GLint result = -1, logLength = -1;
	glGetProgramiv(id, GL_LINK_STATUS, &result);
	if (result == GL_TRUE) return;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
	GLchar log[logLength];
	glGetProgramInfoLog(id, logLength, &logLength, log);
	printf("%s\n", log);
}


// Laddar shader program och skickar dem till grafikkortet
// Inte särskilt intressant faktiskt
GLuint V_LoadShader(const char* name) {
	printf("%s\t", name);
	
	char fragPath[PATH_LENGTH] = {0};
	strcat(fragPath, SYS_GetBasePath());
	strcat(fragPath, name);
	strcat(fragPath, ".fsh");
	char vertPath[PATH_LENGTH] = {0};
	strcat(vertPath, SYS_GetBasePath());
	strcat(vertPath, name);
	strcat(vertPath, ".vsh");
	
	GLuint fragId = glCreateShader(GL_FRAGMENT_SHADER);
	GLuint vertId = glCreateShader(GL_VERTEX_SHADER);
	
	FILE* fragFile = fopen(fragPath, "r");
	FILE* vertFile = fopen(vertPath, "r");

	if (!fragFile || !vertFile) printf("Failed to load shader\n");
	
	int fragLength = 0;
	while (fgetc(fragFile) != EOF) fragLength++;
	rewind(fragFile);
	int vertLength = 0;
	while (fgetc(vertFile) != EOF) vertLength++;
	rewind(vertFile);
	GLchar fragText[fragLength];
	GLchar vertText[vertLength];
	for (int i = 0; i < fragLength; i++) fragText[i] = fgetc(fragFile);
	for (int i = 0; i < vertLength; i++) vertText[i] = fgetc(vertFile);
	
	fclose(fragFile);
	fclose(vertFile);
	
	const char* fragPtr = fragText;
	const char* vertPtr = vertText;
	
	glShaderSource(fragId, 1, &fragPtr, &fragLength);
	glShaderSource(vertId, 1, &vertPtr, &vertLength);
	glCompileShader(fragId);
	glCompileShader(vertId);
	V_CheckShader(fragId);
	V_CheckShader(vertId);
	
	GLuint programId = glCreateProgram();
	glAttachShader(programId, vertId);
	glAttachShader(programId, fragId);
	glLinkProgram(programId);
	V_CheckProgram(programId);
	
	glDetachShader(programId, vertId);
	glDetachShader(programId, fragId);
	glDeleteShader(fragId);
	glDeleteShader(vertId);
	
	printf("\r");
	return programId;
}

void V_RenderModel(Model* m) {
#ifdef V_GL_API
	glCallList(m->list);
#else
	glBindBuffer(GL_ARRAY_BUFFER, m->vertId);

	glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 12);
	glDrawArrays(GL_TRIANGLES, 0, m->numVerts);
#endif
}

void V_SetDepthTesting(bool b) {
	if (b)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);
}

void V_SetDepthWriting(bool b) {
	glDepthMask(b);
}

void V_UseTextures(bool b) {
	if (b)
		glEnable(GL_TEXTURE_2D);
	else
		glDisable(GL_TEXTURE_2D);
}

