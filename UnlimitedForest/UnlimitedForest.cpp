#pragma once

#include "UnlimitedForest.h"
#include "core/input/InputHandler.h"
#include "log/Log.h"

#include <SDL.h>

void catch_gl_error(const std::string& errorMessage) {
	GLenum error = glGetError();
	if (error != GL_NO_ERROR) {
		UF_LOG_ERROR("GLError [{}] : {}\n", error, errorMessage);
	}
}

void main_loop(App& app) {
	InputHandler inputHandler;

	app.start();

	while (app.is_running()) {
		// init and clear screen per loop
		// TODO: this should be somewhere else
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_CULL_FACE);

		EngineConfig cfg = app.get_engine_config();
		glViewport(0, 0, cfg.m_screenWidth, cfg.m_screenHeight);
		catch_gl_error("screen init error");
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);


		if (!inputHandler.update()) {
			app.stop();
		}

		app.update();

		// swap double buffer / update window
		SDL_GL_SwapWindow(app.get_graphics_application_window());
	}
}

int main(int argc, char* argv[]) {
	Log::init();

	App app;

	main_loop(app);

	return 0;
}
