#pragma once

#include "UnlimitedForest.h"
#include "core/application/App.h"
#include "log/Log.h"

int main(int argc, char* argv[]) {
	Log::init();

	App* app = new App();
	app->start();

	delete app;

	return 0;
}
