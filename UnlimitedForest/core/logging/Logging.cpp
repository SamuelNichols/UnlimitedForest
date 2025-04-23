#include "Logging.h"

Logging::Logging() {
	m_loggers.m_infoLogger = spdlog::stdout_color_mt("console");
	m_loggers.m_errorLogger = spdlog::stderr_color_mt("stderr");
}


Loggers Logging::get() {
	return m_loggers;
}

void catch_gl_error(const std::string& errorMessage) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		g_errorLogger->error("Error : {}\n", errorMessage);
	}
}

