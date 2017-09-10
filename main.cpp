#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "gl_helper.h"

typedef struct Vertex {
	glm::vec3 point;
} Vertex;

typedef struct Mesh {
	std::vector<Vertex> verts;
	std::vector<glm::vec3> norms;
	std::vector<u32> indices;
	glm::vec3 pos;
	glm::quat rot;
	glm::vec3 vel;
	glm::vec3 acc;
	glm::vec3 imp;
} Mesh;

Vertex new_vert(glm::vec3 point) {
	Vertex v;
	v.point = point;
	return v;
}

void push_three(std::vector<u32> *indices, u32 x, u32 y, u32 z) {
	 indices->push_back(x);
	 indices->push_back(y);
	 indices->push_back(z);
}

Mesh generate_sphere() {
	Mesh m;

	f32 t = (1.0f + sqrt(5.0f)) / 2.0f;
	m.verts.push_back(new_vert(glm::vec3(-1.0,  t,  0.0)));
	m.verts.push_back(new_vert(glm::vec3( 1.0,  t,  0.0)));
	m.verts.push_back(new_vert(glm::vec3(-1.0, -t,  0.0)));
	m.verts.push_back(new_vert(glm::vec3( 1.0, -t,  0.0)));

	m.verts.push_back(new_vert(glm::vec3( 0.0, -1.0,  t)));
	m.verts.push_back(new_vert(glm::vec3( 0.0,  1.0,  t)));
	m.verts.push_back(new_vert(glm::vec3( 0.0, -1.0, -t)));
	m.verts.push_back(new_vert(glm::vec3( 0.0,  1.0, -t)));

	m.verts.push_back(new_vert(glm::vec3(  t,  0.0, -1.0)));
	m.verts.push_back(new_vert(glm::vec3(  t,  0.0,  1.0)));
	m.verts.push_back(new_vert(glm::vec3( -t,  0.0, -1.0)));
	m.verts.push_back(new_vert(glm::vec3( -t,  0.0,  1.0)));

	push_three(&m.indices, 0, 11, 5);
	push_three(&m.indices, 0, 5, 1);
	push_three(&m.indices, 0, 1, 7);
	push_three(&m.indices, 0, 7, 10);
	push_three(&m.indices, 0, 10, 11);

	push_three(&m.indices, 2, 4, 11);
	push_three(&m.indices, 2, 11, 10);
  	push_three(&m.indices, 2, 6, 10);
	push_three(&m.indices, 2, 3, 4);
	push_three(&m.indices, 2, 3, 6);

	push_three(&m.indices, 7, 8, 6);
	push_three(&m.indices, 7, 8, 1);
	push_three(&m.indices, 1, 9, 5);
	push_three(&m.indices, 1, 8, 9);
	push_three(&m.indices, 4, 3, 9);

	push_three(&m.indices, 4, 5, 11);
	push_three(&m.indices, 4, 5, 9);
	push_three(&m.indices, 3, 8, 9);
	push_three(&m.indices, 3, 6, 8);
	push_three(&m.indices, 10, 7, 6);

	for (u32 i = 0; i < m.indices.size(); i += 3) {
		glm::vec3 u = m.verts[m.indices[i+1]].point - m.verts[m.indices[i]].point;
		glm::vec3 v = m.verts[m.indices[i+2]].point - m.verts[m.indices[i]].point;

		glm::vec3 norm = glm::normalize(glm::cross(u, v));
		m.norms.push_back(norm);
		m.norms.push_back(norm);
		m.norms.push_back(norm);
	}

	m.pos = glm::vec3(0.0, 0.0, 0.0);
	m.rot = glm::quat(1.0, 0.0, 0.0, 0.0);
	m.vel = glm::vec3(0.0, 0.0, 0.0);
	m.acc = glm::vec3(0.0, 0.0, 0.0);
	m.imp = glm::vec3(0.0, 0.0, 0.0);

	return m;
}

int main() {
	SDL_Init(SDL_INIT_VIDEO);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	int screen_height = 480;
	int screen_width = 640;

	SDL_Window *window = SDL_CreateWindow("RollyCow", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, screen_width, screen_height, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
	SDL_GLContext gl_context = SDL_GL_CreateContext(window);
	SDL_GL_GetDrawableSize(window, &screen_width, &screen_height);

	i32 cow_shader = load_and_build_program("vert.vsh", "frag.fsh");
    Mesh sphere = generate_sphere();

	u32 vao = 0;
	GL_CHECK(glGenVertexArrays(1, &vao));
	GL_CHECK(glBindVertexArray(vao));

    u32 a_points = glGetAttribLocation(cow_shader, "point");
	u32 a_norms = glGetAttribLocation(cow_shader, "normal");

	u32 u_model = glGetUniformLocation(cow_shader, "model");
	u32 u_persp = glGetUniformLocation(cow_shader, "persp");
	u32 u_view = glGetUniformLocation(cow_shader, "view");

	GL_CHECK(glEnableVertexAttribArray(a_points));
	GL_CHECK(glEnableVertexAttribArray(a_norms));

	u32 v_sphere_verts;
	GL_CHECK(glGenBuffers(1, &v_sphere_verts));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, v_sphere_verts));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sphere.verts.size() * sizeof(glm::vec3), &sphere.verts[0], GL_STATIC_DRAW));
	GL_CHECK(glVertexAttribPointer(a_points, 3, GL_FLOAT, GL_FALSE, 0, 0));

	u32 v_sphere_norms;
	GL_CHECK(glGenBuffers(1, &v_sphere_norms));
	GL_CHECK(glBindBuffer(GL_ARRAY_BUFFER, v_sphere_norms));
	GL_CHECK(glBufferData(GL_ARRAY_BUFFER, sphere.norms.size() * sizeof(glm::vec3), &sphere.norms[0], GL_STATIC_DRAW));
	GL_CHECK(glVertexAttribPointer(a_norms, 3, GL_FLOAT, GL_FALSE, 0, 0));

    u32 i_sphere;
	GL_CHECK(glGenBuffers(1, &i_sphere));
	GL_CHECK(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_sphere));
	GL_CHECK(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.indices.size() * sizeof(u32), &sphere.indices[0], GL_STATIC_DRAW));

	GL_CHECK(glEnable(GL_DEPTH_TEST));

    f32 current_time = (f32)SDL_GetTicks() / 60.0;
	f32 t = 0.0;

	bool running = true;
	while (running) {
		SDL_Event event;

		SDL_PumpEvents();
        const u8 *state = SDL_GetKeyboardState(NULL);

		sphere.imp = glm::vec3(0.0, 0.0, 0.0);

		f32 impulse = 0.3;
		if (state[SDL_SCANCODE_W]) {
			sphere.imp.y += impulse;
		}
		if (state[SDL_SCANCODE_S]) {
			sphere.imp.y -= impulse;
		}
		if (state[SDL_SCANCODE_A]) {
			sphere.imp.x -= impulse;
		}
		if (state[SDL_SCANCODE_D]) {
			sphere.imp.x += impulse;
		}

		f32 new_time = (f32)SDL_GetTicks() / 60.0;
		f32 dt = new_time - current_time;
		current_time = new_time;
		t += dt;

		f32 friction_x = 0.8;
		f32 friction_y = 0.55;

		sphere.acc.x = (-friction_x) * sphere.vel.x + sphere.imp.x;
		sphere.pos.x += (0.5 * sphere.acc.x * dt * dt) + (sphere.vel.x * dt);
		sphere.vel.x += (sphere.acc.x * dt);

		sphere.acc.y = (-friction_y) * sphere.vel.y + sphere.imp.y;
		sphere.pos.y += (0.5 * sphere.acc.y * dt * dt) + (sphere.vel.y * dt);
		sphere.vel.y += (sphere.acc.y * dt);

		while (SDL_PollEvent(&event)) {
			switch (event.type) {
				case SDL_KEYDOWN: {
					switch (event.key.keysym.sym) {
						case SDLK_ESCAPE: {
							running = false;
						} break;
					}
				} break;
				case SDL_QUIT: {
					running = false;
				} break;
			}
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
        glUseProgram(cow_shader);

		f32 s_ratio = (f32)screen_width / (f32)screen_height;
		glm::mat4 projection = glm::perspective(glm::radians(45.0f), s_ratio, 0.1f, 100.0f);
		glm::mat4 view = glm::lookAt(glm::vec3(0, 0, 25), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));

		glm::mat4 model = glm::mat4(1.0);

		glm::quat rot_y = glm::angleAxis(sphere.vel.y * dt, glm::vec3(-1.0, 0.0, 0.0));
		glm::quat rot_z = glm::angleAxis(sphere.vel.z * dt, glm::vec3(0.0, 0.0, 1.0));
		sphere.rot = rot_x * rot_y * sphere.rot;
		glm::normalize(sphere.rot);
		glm::mat4 rot_mat = glm::mat4_cast(sphere.rot);

		model = glm::translate(model, sphere.pos);
		model *= rot_mat;

		glUniformMatrix4fv(u_persp, 1, GL_FALSE, &projection[0][0]);
		glUniformMatrix4fv(u_view, 1, GL_FALSE, &view[0][0]);
		glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]);

        GL_CHECK(glBindVertexArray(vao));
		GL_CHECK(glDrawElements(GL_TRIANGLES, sphere.indices.size(), GL_UNSIGNED_INT, 0));

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
	return 0;
}
