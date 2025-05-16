#pragma once

#include "EngineConfig.h"
#include "node_manager/NodeManager.h"
#include "camera/Camera.h"
#include "input/InputHandler.h"
#include "log/Log.h"

#include "glad/glad.h"
#include <SDL2/SDL.h>
#include <filesystem>
#include <initializer_list>
#include <string>

class App {
public:
	App();
	~App();

	void start(void);
	void stop(void);
	bool is_running(void) const;

	static App* get() { return m_app; }

	void update(void);

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

	void resize_window(int w, int h);

private:
	void main_loop(void);
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
	InputHandler m_inputHandler;
	bool m_running;

	static App* m_app;
};

inline std::string get_executable_path() {
	char path[MAX_PATH];
	GetModuleFileNameA(NULL, path, MAX_PATH);
	std::string executablePath(path);

	// Get the directory containing the executable
	size_t lastSlash = executablePath.find_last_of("\\/");
	if (lastSlash != std::string::npos) {
		return executablePath.substr(0, lastSlash);
	}

	// Fallback to current directory
	return ".";
}

template<typename... Args>
std::string make_absolute_path(Args... args) {
    std::filesystem::path result = get_executable_path();
    // Fold expression to append each argument to the path
    (result /= ... /= args);
    return result.string();
}

