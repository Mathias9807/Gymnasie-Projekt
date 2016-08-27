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


extern Camera* camera;

GLuint shader = 0;
int vertAttrib = 0, colAttrib = 1, uvAttrib = 2, nrmAttrib = 3;

mat4x4 V_projMat, V_worldMat, V_modelMat;

typedef struct {
	mat4x4 proj, world, model;
} MatrixPair;
List matrixStack;

typedef struct {
	float x, y, z;
	float r, g, b;
	float u, v;
	float nX, nY, nZ;
} Vertex;

void V_StartOpenGL() {
#ifdef V_GL_API
	glewExperimental = GL_TRUE;
	GLenum error = glewInit();
	if (error != GLEW_OK) 
		SYS_Error("GLEW failed to initialize");
#endif

	// Ladda shader programmet
	shader = V_LoadShader("shader");
	glUseProgram(shader);

	vertAttrib = glGetAttribLocation(shader, "vertex_in");
	colAttrib = glGetAttribLocation(shader, "color_in");
	nrmAttrib = glGetAttribLocation(shader, "normal_in");
	uvAttrib = glGetAttribLocation(shader, "uv_in");

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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

	V_SetParam2f("resolution", SYS_GetWidth(), SYS_GetHeight());
}

void V_ClearColor(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

void V_ClearDepth() {
	glClear(GL_DEPTH_BUFFER_BIT);
}

void V_MakeProjection() {
	if (camera == NULL) return;

	double aspect = (double) SYS_GetWidth() / SYS_GetHeight();

	mat4x4_identity(V_projMat);

	mat4x4_perspective(V_projMat, 
			camera->fov, 
			aspect, 
			camera->near, 
			camera->far);

	V_SetParam4m("proj_mat", V_projMat);
}

// Flytta scenen som om den sågs igenom kameran
void V_ApplyCamera() {
	Ship* f = camera->ghost;
	if (f) {
		mat4x4_rotate_X(V_worldMat, V_worldMat, -camera->rOffs[0]);
		mat4x4_rotate_Y(V_worldMat, V_worldMat, -camera->rOffs[1]);

		mat4x4_translate_in_place(V_worldMat, 
			-camera->pOffs[0], -camera->pOffs[1], -camera->pOffs[2]);

		mat4x4 tmp;
		mat4x4_invert(tmp, f->rot);
		mat4x4_mul(V_worldMat, V_worldMat, tmp);

		mat4x4_translate_in_place(V_worldMat, 
			-f->pos[0], -f->pos[1], -f->pos[2]);
			
		mat4x4 inv;
		vec4 v;
		mat4x4_invert(inv, V_worldMat);
		mat4x4_mul_vec4(v, inv, (vec4) {0, 0, 0, 1});
		memcpy(camera->pos, v, sizeof(vec3));
	}else {
		mat4x4_rotate_X(V_worldMat, V_worldMat, -M_PI / 2 + cam.rot[0]);
		mat4x4_rotate_Z(V_worldMat, V_worldMat, +cam.rot[2]);
		mat4x4_rotate_Y(V_worldMat, V_worldMat, +cam.rot[1]);
		
		mat4x4_translate_in_place(V_worldMat, 
			-camera->pos[0], -camera->pos[1], -camera->pos[2]);
	}
}

// Spara alla förflyttningar
void V_PushState() {
	MatrixPair* pair = calloc(1, sizeof(MatrixPair));
	memcpy(&pair->proj, V_projMat, sizeof(mat4x4));
	memcpy(&pair->world, V_worldMat, sizeof(mat4x4));
	memcpy(&pair->model, V_modelMat, sizeof(mat4x4));

	ListAdd(&matrixStack, pair);
}

// Återställ alla förflyttningar sedan förra V_PushState
void V_PopState() {
	MatrixPair* pair = ListGet(&matrixStack, matrixStack.size - 1);
	memcpy(V_projMat, &pair->proj, sizeof(mat4x4));
	memcpy(V_worldMat, &pair->world, sizeof(mat4x4));
	memcpy(V_modelMat, &pair->model, sizeof(mat4x4));

	ListRemove(&matrixStack, matrixStack.size - 1);
}

Model* V_LoadModel(const char* path) {
	Model* m = calloc(1, sizeof(Model));
	glGenBuffers(1, &m->vertId);
	glBindBuffer(GL_ARRAY_BUFFER, m->vertId);

	// Hämta modellens absoluta adress
	char fullPath[PATH_LENGTH] = {0};
	strcat(fullPath, SYS_GetBasePath());
	strcat(fullPath, path);
	
	const struct aiScene* scene = aiImportFile(fullPath, 
					aiProcess_Triangulate |
					aiProcess_JoinIdenticalVertices |
					aiProcess_FlipUVs);

	// Räkna hur många verticeer som finns
	m->numVerts = 0;
	for (int i = 0; i < scene->mNumMeshes; i++) {
		for (int j = 0; j < scene->mMeshes[i]->mNumFaces; j++) 
			m->numVerts += scene->mMeshes[i]->mFaces[j].mNumIndices;
	}

	Vertex* vertBuffer = calloc(1, sizeof(Vertex) * m->numVerts);

	// Håller koll på vilka indexer som redan är fyllda i vertBuffer
	int curVert = 0;

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

				if (mesh->mNumUVComponents[0] > 0)
					uv = mesh->mTextureCoords[0][face.mIndices[k]];

				vertBuffer[curVert++] = (Vertex) {
					vec.x, vec.z, vec.y,
					color[0], color[1], color[2],
					uv.x, uv.y,
					nrm.x, nrm.z, nrm.y
				};
			}
		}
	}
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * m->numVerts, 
		vertBuffer, GL_STATIC_DRAW);

	free(vertBuffer);

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
	mat4x4 tmp, tmp2;
	mat4x4_mul(tmp, V_worldMat, V_modelMat);
	mat4x4_mul(tmp2, V_projMat, tmp);
	V_SetParam4m("proj_mat", tmp2);

	mat4x4_identity(tmp);
	mat4x4_mul(tmp2, tmp, V_modelMat);
	mat4x4_transpose(tmp, tmp2);
	mat4x4_invert(tmp2, tmp);
	V_SetParam4m("norm_mat", tmp2);

	glBindBuffer(GL_ARRAY_BUFFER, m->vertId);

	glVertexAttribPointer(vertAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
	glEnableVertexAttribArray(vertAttrib);
	glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 12);
	glEnableVertexAttribArray(colAttrib);
	glVertexAttribPointer(uvAttrib, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 24);
	glEnableVertexAttribArray(uvAttrib);
	glVertexAttribPointer(nrmAttrib, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*) 32);
	glEnableVertexAttribArray(nrmAttrib);

	glDrawArrays(GL_TRIANGLES, 0, m->numVerts);
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
		V_SetParam1f("textures", 1);
	else
		V_SetParam1f("textures", 0);
}

void V_IsParticle(bool b) {
	if (b)
		V_SetParam1f("particle", 1);
	else
		V_SetParam1f("particle", 0);
	V_UseTextures(b);
}

void V_BindTexture(unsigned id, int pos) {
	glActiveTexture(GL_TEXTURE0 + pos);
	glBindTexture(GL_TEXTURE_2D, id);
}

void V_SetParam4m(const char* var, mat4x4 mat) {
	GLuint id = glGetUniformLocation(shader, var);
	float fArr[16];
	for (int i = 0; i < 16; i++) fArr[i] = mat[i / 4][i % 4];
	glUniformMatrix4fv(id, 1, GL_FALSE, fArr);
}

void V_SetParam1i(const char* var, int i) {
	GLuint id = glGetUniformLocation(shader, var);
	glUniform1i(id, i);
}

void V_SetParam1f(const char* var, float f) {
	GLuint id = glGetUniformLocation(shader, var);
	glUniform1f(id, f);
}

void V_SetParam2f(const char* var, float x, float y) {
	GLuint id = glGetUniformLocation(shader, var);
	glUniform2f(id, x, y);
}

