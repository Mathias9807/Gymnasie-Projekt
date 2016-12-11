#version 100

precision mediump float;

attribute vec3 vertex_in;
attribute vec2 uv_in;

varying vec2 uv;

uniform vec2 pos;
uniform vec2 size;

void main() {
	uv = uv_in;

	vec3 oTo1Vertex =
		vec3(vertex_in.x / 2.0 + 0.5, -vertex_in.z / 2.0 + 0.5, vertex_in.y);

	vec3 vertex = vec3(
		size.x * oTo1Vertex.x + pos.x,
		1.0 - (size.y * oTo1Vertex.y + pos.y),
		oTo1Vertex.y
	);

	gl_Position = vec4(vertex * vec3(2) - vec3(1), 1);
}

