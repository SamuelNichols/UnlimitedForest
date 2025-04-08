#include <SDL.h>
#include <iostream>
#include <algorithm>

#include "InputHandler.h"

// input vars
const float MOVESTEP = 0.005f;

const float SCALESTEP = 0.01f;
const float SCALEMIN = 0.1f;

namespace Core {
	InputHandler::InputHandler() {
		this->m_dragEvent = false;
	}
	// destructor
	InputHandler::~InputHandler() {}

	// called on main loop
	bool InputHandler::update(SelectedItemTransform& transform) {
		while (SDL_PollEvent(&m_event) != 0) {
			if (m_event.type == SDL_QUIT) {
				std::cout << "User exited program. Terminating..." << std::endl;
				return false;
			}
			else if (m_event.type == SDL_MOUSEBUTTONDOWN && m_event.button.button == SDL_BUTTON_LEFT) {
				m_dragEvent = true;
			}
			else if (m_event.type == SDL_MOUSEBUTTONUP && m_event.button.button == SDL_BUTTON_LEFT) {
				m_dragEvent = false;
			}
			else if (m_event.type == SDL_MOUSEMOTION) {
				m_mouseMotion.x = m_event.motion.xrel;
				m_mouseMotion.y = m_event.motion.yrel;
			}
		}
		m_keyState = SDL_GetKeyboardState(NULL);
		if (m_dragEvent) {
			// if left control held, rotate item
			if (m_keyState[SDL_SCANCODE_LCTRL]) {
				handle_rotate_event(transform);
			}
		}
		handle_move_event(transform);
		// make sure to not exit loop (this solution doesn't feel good)
		return true;
	}

	void InputHandler::handle_move_event(SelectedItemTransform& transform) {
		// Retrieve keyboard state
		if (m_keyState[SDL_SCANCODE_UP]) {
			transform.y += MOVESTEP;
			std::cout << "y transform: " << transform.y << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_DOWN]) {
			transform.y -= MOVESTEP;
			std::cout << "y transform: " << transform.y << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_RIGHT]) {
			transform.x += MOVESTEP;
			std::cout << "x transform: " << transform.x << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_LEFT]) {
			transform.x -= MOVESTEP;
			std::cout << "x transform: " << transform.x << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_LSHIFT]) {
			transform.z -= MOVESTEP;
			std::cout << "z transform: " << transform.z << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_SPACE]) {
			transform.z += MOVESTEP;
			std::cout << "z transform: " << transform.z << std::endl;
		}
		if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_EQUALS]) {
			std::cout << "scale up " << std::endl;
			scale(transform,
				transform.scalex + SCALESTEP,
				transform.scaley + SCALESTEP,
				transform.scalez + SCALESTEP
			);
		}
		if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_MINUS]) {
			std::cout << "scale down " << std::endl;
			scale(transform, 
				transform.scalex - SCALESTEP, 
				transform.scaley - SCALESTEP, 
				transform.scalez - SCALESTEP
			);
		}
	}

	void InputHandler::handle_drag_event(SelectedItemTransform& transform) {
		return;
	}

	void InputHandler::handle_rotate_event(SelectedItemTransform& transform) {
		// rotating around the x and y axis thus the rotations are flipped
		transform.roty += m_mouseMotion.x;
		m_mouseMotion.x = 0;
		transform.rotx += m_mouseMotion.y;
		m_mouseMotion.y = 0;
	}

	void InputHandler::scale(SelectedItemTransform& transform, float x, float y, float z) {
		transform.scalex = std::max(x, SCALEMIN);
		transform.scaley = std::max(y, SCALEMIN);
		transform.scalez = std::max(z, SCALEMIN);
	}
}