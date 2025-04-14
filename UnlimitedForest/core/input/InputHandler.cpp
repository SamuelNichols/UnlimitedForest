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
		m_dragEvent = false;
		m_currItem = ITEM;
	}
	// destructor
	InputHandler::~InputHandler() {}

	// called on main loop
	void InputHandler::maybe_swap_selected_item() {
		if (m_event.key.keysym.scancode == SDL_SCANCODE_BACKSLASH && m_event.type == SDL_KEYDOWN && !m_event.key.repeat) {
			++m_currItem;
			switch (m_currItem)
			{
			case Core::ITEM:
				std::cout << "Item selected" << std::endl;
				break;
			case Core::CAMERA:
				std::cout << "Camera selected" << std::endl;
				break;
			default:
				break;
			}
		}
	}

	bool InputHandler::update(SelectedItemTransform& transform, Camera& camera) {
		while (SDL_PollEvent(&m_event)) {
			if (m_event.type == SDL_QUIT) {
				return false;
			}
			maybe_swap_selected_item();
			switch (m_currItem)
			{
			case Core::ITEM:
				// item selected
				InputHandler::update_item(transform);
				break;
			case Core::CAMERA:
				// camera selected
				InputHandler::update_camera(camera);
				break;
			default:
				break;
			}
		}
		return true;
	}

	// -------------------------------- update item flow ---------------------------------------------

	void InputHandler::update_item(SelectedItemTransform& transform) {
		// handle possible events
		switch (m_event.type)
		{
		case SDL_MOUSEBUTTONDOWN:
			if (m_event.button.button == SDL_BUTTON_LEFT) {
				m_dragEvent = true;
			}
			break;

		case SDL_MOUSEBUTTONUP:
			if (m_event.button.button == SDL_BUTTON_LEFT) {
				m_dragEvent = false;
			}
			break;

		case SDL_MOUSEMOTION:
			m_mouseMotion.x = m_event.motion.xrel;
			m_mouseMotion.y = m_event.motion.yrel;
			break;

		default:
			break;

		}

		// handle drag rotate and move events TODO: drag translation functionality
		if (m_dragEvent && m_event.type == SDL_SCANCODE_LCTRL) {
			handle_rotate_event(transform);
		}
		else {
			handle_move_event(transform);
		}
	}

	void InputHandler::handle_move_event(SelectedItemTransform& transform) {
		m_keyState = SDL_GetKeyboardState(NULL);
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

	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end update item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

	// -------------------------------- update camera flow -------------------------------------------

	void InputHandler::update_camera(Camera& camera) {
		// TODO: handle camera input
		m_keyState = SDL_GetKeyboardState(NULL);
		if (m_keyState[SDL_SCANCODE_DOWN]) {
			camera.translate_y(-MOVESTEP);
		}
		if (m_keyState[SDL_SCANCODE_UP]) {
			camera.translate_y(MOVESTEP);
		}
		if (m_keyState[SDL_SCANCODE_LEFT]) {
			camera.translate_x(-MOVESTEP);
		}
		if (m_keyState[SDL_SCANCODE_RIGHT]) {
			camera.translate_x(MOVESTEP);
		}
		if (m_keyState[SDL_SCANCODE_LSHIFT]) {
			camera.translate_z(-MOVESTEP);
		}
		if (m_keyState[SDL_SCANCODE_SPACE]) {
			camera.translate_z(MOVESTEP);
		}
	}

	// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end camera item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


}