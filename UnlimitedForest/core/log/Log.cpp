#include "Log.h"

#include <glad/glad.h>
#include <spdlog/sinks/stdout_color_sinks.h>

std::shared_ptr<spdlog::logger> Log::s_logger;

void Log::init() {
	spdlog::set_pattern("%^[%T] %n: %v%$");
	s_logger = spdlog::stdout_color_mt("UF-LOG");
	s_logger->set_level(spdlog::level::trace);
}

void Log::catch_gl_error(const std::string& errorMessage) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		UF_LOG_ERROR("GLError [{}] : {}", error, errorMessage);
	}
}
