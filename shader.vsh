#version 100

attribute vec3 vertex_in;
attribute vec3 color_in;
attribute vec2 uv_in;
attribute vec3 normal_in;

uniform mat4 proj_mat;
uniform mat4 norm_mat;
uniform vec2 resolution;
uniform float particle;

varying vec4 vertex;
varying vec3 color;
varying vec3 normal;
varying vec2 uv;

void main() {
	vertex = (1.0 - particle) * (proj_mat * vec4(vertex_in, 1));

	vec3 aspect = vec3(resolution.y / resolution.x, 1.0, 1.0);
	vertex += particle * ((proj_mat * vec4(0, 0, 0, 1)) + vec4(vertex_in * aspect, 0));

	color = color_in;
	normal = normalize((norm_mat * vec4(normal_in, 0)).xyz);
	uv = uv_in;

	gl_Position = vertex;
}

