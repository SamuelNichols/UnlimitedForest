// UnlimitedForest.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>	
#include <glad/glad.h>
#include <filesystem>
#include <initializer_list>
#include "core/input/InputHandler.h"
#include "core/camera/Camera.h"
#include "core/logging/Logging.h"
#include "core/node_manager/NodeManager.h"

#include <SDL.h>
#include <cassert>
#include <vector>
#include <fstream>
#include <string>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

struct App {
	int m_screenWidth = 640;
	int m_screenHeight = 480;

	SDL_Window* m_graphicsApplicationWindow = nullptr;
	SDL_GLContext m_openGLContext;

	bool m_running = false;

	GLuint m_graphicsPipelineShaderProgram = 0;

	NodeManager m_nodeManager;

	InputHandler m_inputHandler;
};

void catch_gl_error(const std::string& errorMessage);
void initialize_program(App &app);
void vertex_specification(void);
void create_graphics_pipeline(App &app);
void main_loop(App &app);
void cleanup(App &app);
void get_opengl_version_info(void);
GLuint create_shader_program(std::string& vertexShaderSource, std::string& fragmentShaderSource);
GLuint compile_shader(GLuint type, std::string& source);
std::string load_shader_as_string(const std::string& filename);


template<typename... Args>
std::string make_relative_path(Args... args) {
	std::filesystem::path result;

	// Fold expression to append each argument to the path
	(result /= ... /= args);

	return result.string();
}
