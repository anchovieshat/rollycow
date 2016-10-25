#version 330

in vec3 points;

uniform mat4 model;
uniform mat4 pv;

void main() {
	gl_Position = pv * model * vec4(points, 1.0);
}
