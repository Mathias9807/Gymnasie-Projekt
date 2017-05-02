#version 100

precision mediump float;

varying vec4 vertex;
varying vec3 color;
varying vec3 normal;
varying vec2 uv;

uniform float textures;
uniform sampler2D diffuse_tex; 
uniform mat4 norm_mat;

void main() {
	vec3 diff = vec3(0.1, 0.1, 0.1);

	diff += max(dot(normalize(normal), normalize(vec3(1, 1, 1))), 0.0);

	float alpha = textures * texture2D(diffuse_tex, uv).a;

	gl_FragColor = vec4(texture2D(diffuse_tex, uv).rgb, alpha);

	if (textures < 1.0)
		gl_FragColor = vec4(color * diff, 1);
}

