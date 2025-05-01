#pragma once

#include "UnlimitedForest.h"
#include "core/input/InputHandler.h"
#include "core/camera/Camera.h"
#include "core/node_manager/NodeManager.h"

#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>	
#include <initializer_list>
#include <spdlog/spdlog.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

// OpenGL globals
// OpenGL VAO
GLuint g_vertexArrayObject = 0;
// OpenGL VBO
GLuint g_vertexBufferObject = 0;
// index buffer for reusing shared indecies of triangles
GLuint g_vertexElementBuffer = 0;

// offsets
SelectedItemTransform g_selectedItemTransform;

//global logging object
std::shared_ptr<spdlog::logger> g_infoLogger = nullptr;
std::shared_ptr<spdlog::logger> g_errorLogger = nullptr;

void catch_gl_error(const std::string& errorMessage) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		g_errorLogger->error("GLError [{}] : {}\n", error, errorMessage);
	}
}

void vertex_specification() {
	// lives on CPU
	const std::vector<GLfloat> vertexData{
		//   x      y      z// quad
		// vec 1
		-0.5f, -0.5f, 0.0f, // x y z bottom left
		1.0f, 0.0f, 0.0f,   // r g b
		// vec 2
		0.5f, -0.5f, 0.0f,  // x y z bottom right
		0.0f, 1.0f, 0.0f,   // r g b
		// vec 3
		-0.5f,  0.5f, 0.0f, // x y z top left
		0.0f, 0.0f, 1.0f,   // r g b
		// vec 4
		0.5f, 0.5f, 0.0f,  // x y z top right
		0.0f, 1.0f, 0.0f,   // r g b
	};

	// generating VBO
	glGenBuffers(1, &g_vertexBufferObject);
	// binding buffer to current buffer object id
	glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);
	// setting buffer pointer to current buffer object id to vertex values
	glBufferData(GL_ARRAY_BUFFER,
		vertexData.size() * sizeof(GLfloat),
		vertexData.data(),
		GL_STATIC_DRAW
	);
	catch_gl_error("error generating vbo");

	// enabling vertex attribute for positional data x y z
	// loading to GPU
	// generating VAO
	glGenVertexArrays(1, &g_vertexArrayObject);
	glBindVertexArray(g_vertexArrayObject);
	catch_gl_error("error generating vao");

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,              // 3 components per vertex (x, y, z)
		GL_FLOAT,       // data type
		GL_FALSE,       // not normalized (correct)
		sizeof(GLfloat) * 6,              // stride (0 means tightly packed)
		(GLvoid*)0     // offset of the first component
	);
	catch_gl_error("error setting vertex attributes idx: 0");

	// enabling vertex attribute for color data r g b (offset 3 floats on position)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat) * 6,
		(GLvoid*)(sizeof(GLfloat) * 3)
	);
	catch_gl_error("error setting vertex attributes idx: 1");

	std::vector<GLuint> vectorIdxs = {
		0, 1, 2,
		1, 3, 2
	};
	glGenBuffers(1, &g_vertexElementBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, g_vertexElementBuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
		sizeof(GLfloat) * vectorIdxs.size(),
		vectorIdxs.data(),
		GL_STATIC_DRAW
	);
	catch_gl_error("error loading VEO");
}

void main_loop(App& app) {
	InputHandler inputHandler;

	app.start();

	while (app.is_running()) {

		if (!inputHandler.update()) {
			app.stop();
		}

		predraw(app);

		draw();

		// swap double buffer / update window
		SDL_GL_SwapWindow(app.get_graphics_application_window());
	}
}

void predraw(App& app) {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	
	EngineConfig cfg = app.get_engine_config();
	glViewport(0, 0, cfg.m_screenWidth, cfg.m_screenHeight);
	catch_gl_error("screen init error");
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	GLuint gp = app.get_graphics_pipeline_shader_program();

	glUseProgram(gp);

	// Constant rotation to object
	//g_selectedItemTransform.roty += 0.05f;
	NodeManager* nm = app.get_node_manager();
	Camera* camera = nm->get_camera();
	RenderItem* ri = nm->get_render_item();

	// model transformation by translating our object into world space 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(ri->m_worldPosition.x, ri->m_worldPosition.y, ri->m_worldPosition.z));
	model = glm::rotate(model, glm::radians(ri->m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(ri->m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(ri->m_scale.x, ri->m_scale.y, 1.0f));

	GLint u_ModelMatrixLocation = glGetUniformLocation(gp, "u_ModelMatrix");
	if (u_ModelMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	glm::mat4 view = camera->get_view_matrix();

	GLint u_ViewMatrixLocation = glGetUniformLocation(gp, "u_ViewMatrix");
	if (u_ViewMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Projection matrix (in perspective)
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
		(float)cfg.m_screenWidth / (float)cfg.m_screenHeight,
		0.1f,
		100.0f
	);

	GLint u_PerspectiveLocation = glGetUniformLocation(gp, "u_Perspective");
	if (u_PerspectiveLocation >= 0) {
		glUniformMatrix4fv(u_PerspectiveLocation, 1, GL_FALSE, &perspective[0][0]);
	}
	else {
		std::cout << "could not find u_Perspective uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

}

void draw() {
	glBindVertexArray(g_vertexArrayObject);

	glDrawElements(
		GL_TRIANGLES,
		6,
		GL_UNSIGNED_INT,
		(GLvoid*)0
	);
	catch_gl_error("error drawing triangles");
	// cleanup graphics pipeline
	glUseProgram(0);
}

int main(int argc, char* argv[]) {
	App app;

	// TODO: vertext spec within render object
	vertex_specification();

	main_loop(app);

	return 0;
}
