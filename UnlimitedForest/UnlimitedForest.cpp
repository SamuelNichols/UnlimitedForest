#pragma once

#include "UnlimitedForest.h"

// Logging
Logging g_logging;
Loggers g_loggers = g_logging.get();

// offsets
SelectedItemTransform g_selectedItemTransform;

void initialize_program(App &app) {
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
	app.m_graphicsApplicationWindow = SDL_CreateWindow("Unlimited Forest", 100, 100, app.m_screenWidth, app.m_screenHeight, SDL_WINDOW_OPENGL);
	assert(app.m_graphicsApplicationWindow);

	// initializing opengl context
	app.m_openGLContext = SDL_GL_CreateContext(app.m_graphicsApplicationWindow);
	assert(app.m_openGLContext);

	// init glad, loading OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		std::cerr << "Failed to initialize GLAD" << std::endl;
		cleanup(app);
		std::exit(-1);
	}
	get_opengl_version_info();

	// creating inital nodes
	app.m_nodeManager.create_camera();
	app.m_nodeManager.create_render_item(glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	// keep mouse in center
	SDL_WarpMouseInWindow(app.m_graphicsApplicationWindow, app.m_screenWidth / 2, app.m_screenHeight / 2);
	SDL_SetRelativeMouseMode(SDL_TRUE);
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

void create_graphics_pipeline(App &app) {
	const std::string vertexShaderSource = load_shader_as_string(make_relative_path("shaders", "vert.glsl"));
	const std::string fragmentShaderSource = load_shader_as_string(make_relative_path("shaders", "frag.glsl"));
	app.m_graphicsPipelineShaderProgram = create_shader_program(vertexShaderSource, fragmentShaderSource);

	catch_gl_error("error creating graphics pipeline");
}

void main_loop(App &app) {
	app.m_running = true;
	while (app.m_running) {

		app.m_running = app.m_inputHandler.update(app.m_nodeManager, app.m_screenWidth, app.m_screenHeight);
		//bool n = gApp.m_nodeManager.update();

		predraw(app);

		draw();
		// update the window
		SDL_GL_SwapWindow(app.m_graphicsApplicationWindow);
	}
}

void cleanup(App &app) {
	// cleanup sdl window and opengl context
	SDL_GL_DeleteContext(app.m_openGLContext);
	SDL_DestroyWindow(app.m_graphicsApplicationWindow);
	SDL_Quit();
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
	App app;
	initialize_program(app);
	vertex_specification();
	create_graphics_pipeline(app);
	main_loop(app);
	cleanup(app);
	return 0;
}
