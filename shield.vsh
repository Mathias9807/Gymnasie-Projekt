#version 100

precision mediump float;

attribute vec3 vertex_in;
attribute vec3 color_in;
attribute vec2 uv_in;
attribute vec3 normal_in;

uniform mat4 proj_mat, world_mat;

varying vec4 vertex, worldVertex, localVertex;
varying vec2 uv;

void main() {
	localVertex = vec4(vertex_in, 1);
	worldVertex = world_mat * localVertex;
	vertex = proj_mat * localVertex;

	uv = uv_in;

	gl_Position = vertex;
}

