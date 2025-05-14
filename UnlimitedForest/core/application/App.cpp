#include "App.h"

App* App::m_app = nullptr;

App::App() {
	m_graphicsPipelineShaderProgram = 0;
	m_running = false;
	initialize_sdl();
	create_graphics_pipeline();

	m_app = this;
}

App::~App() {
	UF_LOG_INFO("app deconstruction");
	cleanup();
}

void App::initialize_sdl(){
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
	Uint32 flags = SDL_WINDOW_OPENGL  
		| SDL_WINDOW_RESIZABLE
		| SDL_WINDOW_ALLOW_HIGHDPI;
	m_graphicsApplicationWindow = SDL_CreateWindow("Unlimited Forest", 100, 100, m_engineConfig.m_screenWidth, m_engineConfig.m_screenHeight, flags);
	assert(m_graphicsApplicationWindow);

	// initializing opengl context
	m_openGLContext = SDL_GL_CreateContext(m_graphicsApplicationWindow);
	assert(m_openGLContext);

	// init glad, loading OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
		UF_LOG_ERROR("Failed to initialize GLAD");
		cleanup();
		std::exit(-1);
	}
	get_opengl_version_info();

	// creating inital nodes
	m_nodeManager.create_camera();
	// TODO: find a better place to define render objects
	m_nodeManager.create_render_item(
		{
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
		},
		{
		0, 1, 2,
		1, 3, 2
		},
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(0.0f, 0.0f, 0.0f),
		glm::vec3(1.0f, 1.0f, 1.0f)
	);

	// keep mouse in center
	SDL_WarpMouseInWindow(m_graphicsApplicationWindow, m_engineConfig.m_screenWidth / 2, m_engineConfig.m_screenHeight / 2);
	SDL_SetRelativeMouseMode(SDL_TRUE);
}

void App::get_opengl_version_info() {
	const char* vendor = reinterpret_cast<const char*>(glGetString(GL_VENDOR));
	const char* renderer = reinterpret_cast<const char*>(glGetString(GL_RENDERER));
	const char* version = reinterpret_cast<const char*>(glGetString(GL_VERSION));
	const char* glsl = reinterpret_cast<const char*>(glGetString(GL_SHADING_LANGUAGE_VERSION));
	UF_LOG_INFO("\nVendor: {}\nRenderer: {}\nVersion: {}\nShading Language: {}\n",
		vendor,
		renderer,
		version,
		glsl
	);
}

void App::cleanup() {
	// cleanup sdl window and opengl context
	SDL_GL_DeleteContext(m_openGLContext);
	SDL_DestroyWindow(m_graphicsApplicationWindow);
	SDL_Quit();
}

void App::start() {
	m_running = true;
}

void App::stop() {
	m_running = false;
}

bool App::is_running() const {
	return m_running;
}

void App::update() {
	m_nodeManager.update();
}

// TODO: move this to graphics pipeline / shader handler
GLuint App::compile_shader(GLuint type, const std::string& source) {
	GLuint shaderObject = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(shaderObject, 1, &src, nullptr);
	glCompileShader(shaderObject);

	//catch_gl_error("error compiling shader object");

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

GLuint App::create_shader_program(const std::string& vertexShaderSource, const std::string& fragmentShaderSource) {
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

void App::create_graphics_pipeline() {
	const std::string vertexShaderSource = load_shader_as_string(make_relative_path("shaders", "vert.glsl"));
	const std::string fragmentShaderSource = load_shader_as_string(make_relative_path("shaders", "frag.glsl"));
	m_graphicsPipelineShaderProgram = create_shader_program(vertexShaderSource, fragmentShaderSource);

	//catch_gl_error("error creating graphics pipeline");
}

void App::resize_window(int w, int h) {
	UF_LOG_INFO("hit {}|{}", w, h);
	SDL_SetWindowSize(m_graphicsApplicationWindow, w, h);
}

std::string App::load_shader_as_string(const std::string& filename) {
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
