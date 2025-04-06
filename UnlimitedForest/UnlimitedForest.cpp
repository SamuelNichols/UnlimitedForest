#pragma once

#include "UnlimitedForest.h"
#include <SDL.h>
#include <glad/glad.h>
#include <iostream>
#include <cassert>
#include <vector>
#include <fstream>
#include <string>
#include <filesystem>	
#include <initializer_list>

// screen globals
int g_screenWidth = 640;
int g_screenHeight = 480;
SDL_Window* g_graphicsApplicationWindow = nullptr;
SDL_GLContext g_openGLContext;

// engine loop
bool g_running = false;

// OpenGL globals
// OpenGL VAO
GLuint g_vertexArrayObject = 0;
// OpenGL VBO
GLuint g_vertexBufferObject = 0;
// program object for shaders
GLuint g_graphicsPipelineShaderProgram = 0;

void initialize_program() {
	//initializing sdl video component
	assert(SDL_Init(SDL_INIT_VIDEO) >= 0);

	// opengl version 4.1
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
	// only allowing non-depricated functionality
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	// render flags
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	// creating the window handler
	g_graphicsApplicationWindow = SDL_CreateWindow("Unlimited Forest", 100, 100, g_screenWidth, g_screenHeight, SDL_WINDOW_OPENGL);
	assert(g_graphicsApplicationWindow);

	// initializing opengl context
	g_openGLContext = SDL_GL_CreateContext(g_graphicsApplicationWindow);
	assert(g_openGLContext);

	// init glad, loading OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		cleanup();
		std::exit(-1);
	}
	get_opengl_version_info();
}

void vertex_specification() {
	// lives on CPU
	const std::vector<GLfloat> vertexData{
		//   x      y      z
		-0.8f, -0.8f, 0.0f, // x y z
		1.0f, 0.0f, 0.0f,   // r g b
		0.8f, -0.8f, 0.0f,  // x y z
		0.0f, 1.0f, 0.0f,   // r g b
		0.0f,  0.8f, 0.0f,  // x y z
		0.0f, 0.0f, 1.0f    // r g b
	};
	// loading to GPU
	// generating VAO
	glGenVertexArrays(1, &g_vertexArrayObject);
	glBindVertexArray(g_vertexArrayObject);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in vertex_specification vao: " << error << std::endl;
	}

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
	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in buffer init: " << error << std::endl;
	}

	// enabling vertex attribute for positional data x y z
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0,
		3,              // 3 components per vertex (x, y, z)
		GL_FLOAT,       // data type
		GL_FALSE,       // not normalized (correct)
		sizeof(GLfloat) * 6,              // stride (0 means tightly packed)
		(GLvoid*)0     // offset of the first component
	);
	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in vertex_specification possition: " << error << std::endl;
	}

	// enabling vertex attribute for color data r g b (offset 3 floats on position)
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1,
		3,
		GL_FLOAT,
		GL_FALSE,
		sizeof(GLfloat) * 6,
		(GLvoid*)(sizeof(GLfloat) * 3)
	);

	error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in vertex_specification color: " << error << std::endl;
	}
}

GLuint compile_shader(GLuint type, const std::string& source) {
	GLuint shaderObject = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "\nError in compile_shader\n" << "Shader: " << src << "Error: " << error << std::endl;
	}

	// Check if compilation succeeded
	GLint status;
	glGetShaderiv(shaderObject, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetShaderiv(shaderObject, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetShaderInfoLog(shaderObject, length, nullptr, log.data());
		std::cerr << "Shader compilation failed (type "
			<< (type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< "): " << log.data() << std::endl;
		glDeleteShader(shaderObject);
		return 0;
	}

	return shaderObject;
}

GLuint create_shader_program(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
	GLuint programObject = glCreateProgram();

	GLuint myVertexShader = compile_shader(GL_VERTEX_SHADER, vertexShaderSource);
	GLuint myFragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentShaderSource);

	if (!myVertexShader || !myFragmentShader) {
		glDeleteProgram(programObject);
		std::cerr << "Failed to build shader program" << std::endl;
		return 0;
	}

	glAttachShader(programObject, myVertexShader);
	glAttachShader(programObject, myFragmentShader);
	glLinkProgram(programObject);

	// Check if linking succeeded
	GLint status;
	glGetProgramiv(programObject, GL_LINK_STATUS, &status);
	if (status == GL_FALSE) {
		GLint length;
		glGetProgramiv(programObject, GL_INFO_LOG_LENGTH, &length);
		std::vector<char> log(length);
		glGetProgramInfoLog(programObject, length, nullptr, log.data());
		std::cerr << "Program linking failed: " << log.data() << std::endl;
		glDeleteProgram(programObject);
		return 0;
	}

	// Clean up
	glDetachShader(programObject, myVertexShader);
	glDetachShader(programObject, myFragmentShader);
	glDeleteShader(myVertexShader);
	glDeleteShader(myFragmentShader);

	return programObject;
}

void create_graphics_pipeline() {
	const std::string vertexShaderSource = load_shader_as_string(make_relative_path("shaders", "vert.glsl"));
	const std::string fragmentShaderSource = load_shader_as_string(make_relative_path("shaders", "frag.glsl"));
	g_graphicsPipelineShaderProgram = create_shader_program(vertexShaderSource, fragmentShaderSource);

	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in create_graphics_pipeline: " << error << std::endl;
	}
}

void main_loop() {
	g_running = true;
	while (g_running) {
		input();

		predraw();

		draw();

		// update the window
		SDL_GL_SwapWindow(g_graphicsApplicationWindow);
	}
}

void cleanup() {
	// cleanup sdl window and opengl context
	SDL_GL_DeleteContext(g_openGLContext);
	SDL_DestroyWindow(g_graphicsApplicationWindow);
	SDL_Quit();
}

void input() {
	SDL_Event e;
	while (SDL_PollEvent(&e) != 0) {
		if (e.type == SDL_QUIT) {
			std::cout << "User exited program. Terminating..." << std::endl;
			g_running = false;
		}
	}

}

void predraw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, g_screenWidth, g_screenHeight);
	glClearColor(1.f, 1.f, 0.f, 1.f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(g_graphicsPipelineShaderProgram);
}

void draw() {
	glBindVertexArray(g_vertexArrayObject);
	glBindBuffer(GL_ARRAY_BUFFER, g_vertexBufferObject);

	glDrawArrays(GL_TRIANGLES, 0, 3);
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		std::cerr << "OpenGL error in " << "draw" << ": " << error << std::endl;
	}
	// cleanup graphics pipeline
	glUseProgram(0);
}

void get_opengl_version_info() {
	std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
	std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Shading Language: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
}

std::string load_shader_as_string(const std::string& filename) {
	// Open the file
	std::ifstream file(filename);
	if (!file.is_open()) {
		std::cerr << "Error: could not open file: " << filename << std::endl;
		return "";
	}

	// Use a string stream to read the file contents
	std::stringstream shaderStream;
	shaderStream << file.rdbuf();

	// No need to manually close, as the ifstream destructor will handle it.
	return shaderStream.str();
}

int main(int argc, char* argv[]) {
	initialize_program();
	vertex_specification();
	create_graphics_pipeline();
	main_loop();
	cleanup();
	return 0;
}
