// UnlimitedForest.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include <iostream>	
#include <glad/glad.h>

void initialize_program(void);
void vertex_specification(void);
void create_graphics_pipeline(void);
void main_loop(void);
void cleanup(void);
void input(void);
void predraw(void);
void draw(void);
void get_opengl_version_info(void);
GLuint create_shader_program(std::string &vertexShaderSource, std::string &fragmentShaderSource);
GLuint compile_shader(GLuint type, std::string& source);
