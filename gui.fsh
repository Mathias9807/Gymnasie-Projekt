#version 100

precision mediump float;

varying vec2 uv;

uniform sampler2D tex; 
uniform vec2 subPos;
uniform vec2 subSize;

void main() {
	gl_FragColor = texture2D(tex, uv * subSize + subPos);
}

