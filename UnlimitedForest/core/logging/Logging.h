#pragma once

#include <glad/glad.h>
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

struct Loggers {
	std::shared_ptr<spdlog::logger> m_infoLogger = nullptr;
	std::shared_ptr<spdlog::logger> m_errorLogger = nullptr;
};

class Logging {
public:

	Logging();
	~Logging() {};

	Loggers get(void);

private:
	Loggers m_loggers;
};

void catch_gl_error(const std::string& errorMessage);
