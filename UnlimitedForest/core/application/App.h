#pragma once

#include "EngineConfig.h"
#include "../node_manager/NodeManager.h"
#include "../camera/Camera.h"

#include "glad/glad.h"
#include <SDL.h>
#include <memory>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <filesystem>
#include <initializer_list>
#include <iostream>	

#include <cassert>
#include <vector>
#include <fstream>
#include <string>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

extern std::shared_ptr<spdlog::logger> g_infoLogger;
extern std::shared_ptr<spdlog::logger> g_errorLogger;

class App {
public:
	App();
	~App();

	void start(void);
	void stop(void);
	bool is_running(void) const;

	static App* get() { return m_app; }

	NodeManager* get_node_manager() {
		return &m_nodeManager;
	}

	SDL_Window* get_graphics_application_window() {
		return m_graphicsApplicationWindow;
	}

	GLuint& get_graphics_pipeline_shader_program() {
		return m_graphicsPipelineShaderProgram;
	}
	EngineConfig& get_engine_config() {
		return m_engineConfig;
	}

private:
	void initialize_logger(void);
	void initialize_sdl(void);
	void get_opengl_version_info(void);
	void cleanup(void);

	// TODO: move this to graphics pipeline / shader handler
	GLuint compile_shader(GLuint type, const std::string& source);
	GLuint create_shader_program(const std::string& vertexShaderSource, const std::string& fragmentShaderSource);
	void create_graphics_pipeline(void);
	std::string load_shader_as_string(const std::string& filename);

	EngineConfig m_engineConfig;
	SDL_Window* m_graphicsApplicationWindow;
	SDL_GLContext m_openGLContext;
	GLuint m_graphicsPipelineShaderProgram;
	NodeManager m_nodeManager;
	bool m_running;

	static App* m_app;
};

template<typename... Args>
std::string make_relative_path(Args... args) {
	std::filesystem::path result;

	// Fold expression to append each argument to the path
	(result /= ... /= args);

	return result.string();
}
