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
#include <spdlog/sinks/stdout_color_sinks.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/matrix.hpp>
#include <glm/vec3.hpp>
#include <glm/mat4x4.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>


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
// index buffer for reusing shared indecies of triangles
GLuint g_vertexElementBuffer = 0;
// program object for shaders
GLuint g_graphicsPipelineShaderProgram = 0;

// offsets
SelectedItemTransform g_selectedItemTransform;

// global for now since rendering is still in main process
NodeManager g_nodeManager;

//global logging object
std::shared_ptr<spdlog::logger> g_infoLogger = nullptr;
std::shared_ptr<spdlog::logger> g_errorLogger = nullptr;

void initialize_logger() {
	// create a color multi-threaded logger
	g_infoLogger = spdlog::stdout_color_mt("console");
	g_errorLogger = spdlog::stderr_color_mt("stderr");
}

void catch_gl_error(const std::string& errorMessage) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		//std::cerr << errorMessage << "\nError " << " : " << error << std::endl;
		g_errorLogger->error("Error : {}\n", errorMessage);
	}
}

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

	// creating inital nodes
	g_nodeManager.create_camera();
	g_nodeManager.create_render_item(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	// keep mouse in center
	SDL_WarpMouseInWindow(g_graphicsApplicationWindow, g_screenWidth / 2, g_screenHeight / 2);
	SDL_SetRelativeMouseMode(SDL_TRUE);
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

GLuint compile_shader(GLuint type, const std::string& source) {
	GLuint shaderObject = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	catch_gl_error("error compiling shader object");

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

	catch_gl_error("error creating graphics pipeline");
}

void main_loop() {
	InputHandler inputHandler;

	g_running = true;
	while (g_running) {

		g_running = inputHandler.update(g_nodeManager);
		//bool n = g_nodeManager.update();

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

void predraw() {
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	glViewport(0, 0, g_screenWidth, g_screenHeight);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

	glUseProgram(g_graphicsPipelineShaderProgram);

	// Constant rotation to object
	//g_selectedItemTransform.roty += 0.05f;

	Camera* camera = g_nodeManager.get_camera();
	RenderItem* ri = g_nodeManager.get_render_item();

	// model transformation by translating our object into world space 
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(ri->m_worldPosition.x, ri->m_worldPosition.y, ri->m_worldPosition.z));
	model = glm::rotate(model, glm::radians(ri->m_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
	model = glm::rotate(model, glm::radians(ri->m_rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
	model = glm::scale(model, glm::vec3(ri->m_scale.x, ri->m_scale.y, 1.0f));

	GLint u_ModelMatrixLocation = glGetUniformLocation(g_graphicsPipelineShaderProgram, "u_ModelMatrix");
	if (u_ModelMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ModelMatrixLocation, 1, GL_FALSE, &model[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	glm::mat4 view = camera->get_view_matrix();

	GLint u_ViewMatrixLocation = glGetUniformLocation(g_graphicsPipelineShaderProgram, "u_ViewMatrix");
	if (u_ViewMatrixLocation >= 0) {
		glUniformMatrix4fv(u_ViewMatrixLocation, 1, GL_FALSE, &view[0][0]);
	}
	else {
		std::cout << "could not find u_ModelMatrix uniform" << std::endl;
		exit(EXIT_FAILURE);
	}

	// Projection matrix (in perspective)
	glm::mat4 perspective = glm::perspective(glm::radians(45.0f),
		(float)g_screenWidth / (float)g_screenHeight,
		0.1f,
		100.0f
	);

	GLint u_PerspectiveLocation = glGetUniformLocation(g_graphicsPipelineShaderProgram, "u_Perspective");
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

void get_opengl_version_info() {
	const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	g_infoLogger->info("\nVendor: {}\nRenderer: {}\nVersion: {}\nShading Language: {}\n",
		vendor,
		renderer,
		version,
		glsl
	);
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
	initialize_logger();
	initialize_program();
	vertex_specification();
	create_graphics_pipeline();
	main_loop();
	cleanup();
	return 0;
}
