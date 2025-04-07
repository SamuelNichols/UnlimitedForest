// UnlimitedForest.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>	
#include <glad/glad.h>
#include <filesystem>
#include <initializer_list>

void catch_gl_error(const std::string& errorMessage);
void initialize_program(void);
void vertex_specification(void);
void create_graphics_pipeline(void);
void main_loop(void);
void cleanup(void);
void input(void);
void predraw(void);
void draw(void);
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
