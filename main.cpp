#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <OpenGL/gl3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stdio.h>
#include <stdlib.h>
#include <vector>

#include "gl_helper.h"

typedef struct Vertex {
	glm::vec3 point;
} Vertex;

typedef struct Mesh {
	std::vector<Vertex> verts;
	std::vector<glm::vec3> indices;
} Mesh;

Vertex new_vert(glm::vec3 point) {
	Vertex v;
	v.point = point;
	return v;
}

Mesh generate_sphere() {
	Mesh m;

	f32 t = (1.0 + sqrt(5.0)) / 2.0;

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

	m.indices.push_back(glm::vec3(0, 11, 5));
	m.indices.push_back(glm::vec3(0, 5, 1));
	m.indices.push_back(glm::vec3(0, 1, 7));
	m.indices.push_back(glm::vec3(0, 7, 10));
	m.indices.push_back(glm::vec3(0, 10, 11));

	m.indices.push_back(glm::vec3(1, 5, 9));
	m.indices.push_back(glm::vec3(5, 11, 4));
	m.indices.push_back(glm::vec3(11, 10, 2));
	m.indices.push_back(glm::vec3(10, 7, 6));
	m.indices.push_back(glm::vec3(7, 1, 8));

	m.indices.push_back(glm::vec3(3, 9, 4));
	m.indices.push_back(glm::vec3(3, 4, 2));
	m.indices.push_back(glm::vec3(3, 2, 6));
	m.indices.push_back(glm::vec3(3, 6, 8));
	m.indices.push_back(glm::vec3(3, 8, 9));

	m.indices.push_back(glm::vec3(4, 9, 5));
	m.indices.push_back(glm::vec3(2, 4, 11));
	m.indices.push_back(glm::vec3(6, 2, 10));
	m.indices.push_back(glm::vec3(8, 6, 7));
	m.indices.push_back(glm::vec3(9, 8, 1));
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
    Mesh mesh = generate_sphere();

	u32 vao = 0;
	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

    u32 a_points = glGetAttribLocation(cow_shader, "points");

	u32 u_model = glGetUniformLocation(cow_shader, "model");
	u32 u_pv = glGetUniformLocation(cow_shader, "pv");

	glEnableVertexAttribArray(a_points);

    u32 i_mesh;
	glGenBuffers(1, &i_mesh);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, i_mesh);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.indices.size() * sizeof(glm::vec3), &mesh.indices[0], GL_STATIC_DRAW);

	u32 v_mesh;
	glGenBuffers(1, &v_mesh);
	glBindBuffer(GL_ARRAY_BUFFER, v_mesh);
	glBufferData(GL_ARRAY_BUFFER, mesh.verts.size() * sizeof(glm::vec3), &mesh.verts[0], GL_STATIC_DRAW);
	glVertexAttribPointer(a_points, 3, GL_FLOAT, GL_FALSE, 0, 0);

	bool running = true;
	while (running) {
		SDL_Event event;

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
		glm::mat4 projection = glm::mat4(1.0);
		glm::mat4 view = glm::mat4(1.0);
		glm::mat4 pv = projection * view;

		glm::mat4 model = glm::mat4(1.0);

		glUniformMatrix4fv(u_pv, 1, GL_FALSE, &pv[0][0]);
		glUniformMatrix4fv(u_model, 1, GL_FALSE, &model[0][0]);

        glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, mesh.indices.size() * 3, GL_UNSIGNED_INT, &mesh.indices[0]);

		SDL_GL_SwapWindow(window);
	}

	SDL_GL_DeleteContext(gl_context);
	SDL_Quit();
	return 0;
}
