#version 100

precision mediump float;

varying vec4 vertex, worldVertex, localVertex;
varying vec2 uv;

uniform sampler2D diffuse_tex; 
uniform float alpha;

uniform vec3 hlCenter; // Highlight information
uniform float hlTime;
uniform float health;

void main() {
	float hl = 0.0;

	vec3 d = localVertex.xyz - hlCenter;
	float hlRadius2 = d.x*d.x + d.y*d.y + d.z*d.z;
	float radius = pow(8.0*hlTime, 2.0)-hlRadius2;
	hl = pow(1.0 / (3.0 * abs(radius)), 2.0);

	if (hlCenter == vec3(0.0)) hl = 0.0;
	else if (health <= 1.0 && radius > 0.0) discard;

	gl_FragColor = vec4(texture2D(diffuse_tex, uv).rgb, step(1.0, health)
		* alpha * (1.0 + hl));
}

