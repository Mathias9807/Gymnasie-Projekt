#version 100

attribute vec3 vertex_in;
attribute vec3 color_in;

varying vec3 color;

void main() {
	gl_Position = vec4(vertex_in * 0.8 * vec3(1, 1, -1), 1);
	color = color_in;
}

