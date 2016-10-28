#version 330

in vec3 point;
in vec3 normal;

uniform mat4 model;
uniform mat4 persp;
uniform mat4 view;

out vec3 f_position;
out vec3 f_normal;

void main() {
	gl_Position = persp * view * model * vec4(point, 1.0);
	f_position = point;
	f_normal = transpose(inverse(mat3(persp * view))) * normal;
}
