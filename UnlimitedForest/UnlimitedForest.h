// UnlimitedForest.h : Include file for standard system include files,
// or project specific include files.

#pragma once

#include "core/application/EngineConfig.h"
#include "core/application/App.h"

#include <iostream>	
#include <glad/glad.h>

void catch_gl_error(const std::string& errorMessage);
void vertex_specification();
void main_loop(App& app);
void predraw(App& app);
void draw(void);

