#pragma once

#include <memory>
#include <spdlog/spdlog.h>

class Log {
public:
	static void init(void);

	inline static std::shared_ptr<spdlog::logger>& getLogger() { return  s_logger; }

	static void catch_gl_error(const std::string& errorMessage);

private:
	static std::shared_ptr<spdlog::logger> s_logger;
};

#define UF_LOG_TRACE(...)      ::Log::getLogger()->trace(__VA_ARGS__)
#define UF_LOG_DEBUG(...)      ::Log::getLogger()->debug(__VA_ARGS__)
#define UF_LOG_INFO(...)       ::Log::getLogger()->info(__VA_ARGS__)
#define UF_LOG_WARN(...)       ::Log::getLogger()->warn(__VA_ARGS__)
#define UF_LOG_ERROR(...)      ::Log::getLogger()->error(__VA_ARGS__)
#define UF_LOG_CRITICAL(...)   ::Log::getLogger()->critical(__VA_ARGS__)

#define CATCH_GL_ERROR(...)    ::Log::catch_gl_error(__VA_ARGS__)
