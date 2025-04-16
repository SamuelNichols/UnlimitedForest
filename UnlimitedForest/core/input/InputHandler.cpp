#include <SDL.h>
#include <iostream>
#include <algorithm>

#include "InputHandler.h"

// input vars
const float MOVESTEP = 0.005f;

const float SCALESTEP = 0.01f;
const float SCALEMIN = 0.1f;


InputHandler::InputHandler() {
	m_dragEvent = false;
	m_currItem = ITEM;
}
// destructor
InputHandler::~InputHandler() {}

// called on main loop
bool InputHandler::update(NodeManager& nm) {
	while (SDL_PollEvent(&m_event)) {
		if (m_event.type == SDL_KEYDOWN && !m_event.key.repeat) {
			if (m_event.key.keysym.scancode == SDL_SCANCODE_BACKSLASH) {
				++m_currItem;
			}
		}
	}
	Camera* cam = nullptr;
	RenderItem* ri = nullptr;
	switch (m_currItem)
	{
	case ITEM:
		// item selected
		std::cout << "item selected" << std::endl;
		ri = nm.get_render_item();
		if (ri) {
			return InputHandler::update_item(ri);
		}
		return false;
		break;
	case CAMERA:
		// camera selected
		std::cout << "camera selected" << std::endl;
		cam = nm.get_camera();
		if (cam) {
			return InputHandler::update_camera(cam);
		}
		return false;
		break;
	default:
		break;
	}
	return false;
}

// -------------------------------- update item flow ---------------------------------------------

bool InputHandler::update_item(RenderItem* ri) {
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
			handle_rotate_event(ri);
		}
	}
	handle_move_event(ri);
	handle_scale_event(ri);
	// make sure to not exit loop (this solution doesn't feel good)
	return true;
}

void InputHandler::handle_move_event(RenderItem* ri) {
	glm::vec3 transform(0.0f, 0.0f, 0.0f);
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
	// apply the transform to the render object
	ri->translate(transform);
}

void InputHandler::handle_scale_event(RenderItem* ri) {
	if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_EQUALS]) {
		std::cout << "scale up " << std::endl;
		ri->scale(glm::vec3(SCALESTEP, SCALESTEP, SCALESTEP));
	}
	if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_MINUS]) {
		std::cout << "scale down " << std::endl;
		ri->scale(glm::vec3(-SCALESTEP, -SCALESTEP, -SCALESTEP));
	}
}

void InputHandler::handle_drag_event(RenderItem* ri) {
	return;
}

void InputHandler::handle_rotate_event(RenderItem* ri) {
	glm::vec3 rotation(0.0f, 0.0f, 0.0f);
	// rotating around the x and y axis thus the rotations are flipped
	rotation.y += m_mouseMotion.x;
	m_mouseMotion.x = 0;
	rotation.x += m_mouseMotion.y;
	m_mouseMotion.y = 0;
	// TODO: handle z rotate one day

	ri->rotate(rotation);
}

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end update item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

// -------------------------------- update camera flow -------------------------------------------

bool InputHandler::update_camera(Camera* camera) {
	// TODO: handle camera input
	m_keyState = SDL_GetKeyboardState(NULL);
	if (m_keyState[SDL_SCANCODE_DOWN]) {
		camera->translate_y(-MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_UP]) {
		camera->translate_y(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_LEFT]) {
		camera->translate_x(-MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_RIGHT]) {
		camera->translate_x(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_LSHIFT]) {
		camera->translate_z(-MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_SPACE]) {
		camera->translate_z(MOVESTEP);
	}
	return true;
}

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end camera item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


