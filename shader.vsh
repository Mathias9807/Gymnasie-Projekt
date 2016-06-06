#version 100

attribute vec3 vertex_in;
attribute vec3 color_in;
attribute vec2 uv_in;
attribute vec3 normal_in;

uniform mat4 proj_mat;
uniform mat4 norm_mat;

varying vec4 vertex;
varying vec3 color;
varying vec3 normal;
varying vec2 uv;
varying float textures;

void main() {
	vertex = proj_mat * vec4(vertex_in, 1);
	color = color_in;
	normal = normalize((norm_mat * vec4(normal_in, 0)).xyz);
	uv = uv_in;

	gl_Position = vertex;
}

