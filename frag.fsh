#version 330

in vec3 f_position;
in vec3 f_normal;

out vec4 color;

void main() {
	vec3 light = vec3(5.0, 5.0, 10.0);

	float brightness = dot(f_normal, normalize(light));
	vec3 dark = (normalize(f_position) / 2) + vec3(0.2, 0.2, 0.2);
	vec3 reg = normalize(f_position) + vec3(0.2, 0.2, 0.2);

	color = vec4(mix(dark, reg, brightness), 1.0);
}
