#include <SDL.h>
#include <iostream>
#include <algorithm>

#include "InputHandler.h"
#include "../core/application/App.h"


InputHandler::InputHandler() {
	m_dragEvent = false;
	m_currItem = ITEM;
}
// destructor
InputHandler::~InputHandler() {}

// called on main loop
bool InputHandler::update() {
	App* app = App::get();
	NodeManager* nm = app->get_node_manager();
	while (SDL_PollEvent(&m_event)) {
		if (m_event.type == SDL_QUIT) {
			std::cout << "User exited program. Terminating..." << std::endl;
			return false;
		}
		if (m_event.type == SDL_KEYDOWN && !m_event.key.repeat) {
			if (m_event.key.keysym.scancode == SDL_SCANCODE_BACKSLASH) {
				++m_currItem;
				switch (m_currItem)
				{
				case ITEM:
					std::cout << "item selected" << std::endl;
					break;
				case CAMERA:
					std::cout << "camera selected" << std::endl;
					break;
				case END:
					break;
				default:
					break;
				}
			}
		}
		if (m_currItem == ITEM) {
			RenderItem* ri = nullptr;
			ri = nm->get_render_item();
			if (ri) {
				return InputHandler::update_item(ri);
			}
			return false;
		}
		else if (m_currItem == CAMERA) {
			EngineConfig cfg = app->get_engine_config();
			Camera* cam = nullptr;
			cam = nm->get_camera();
			if (cam) {
				return InputHandler::update_camera(cam, cfg.m_screenWidth, cfg.m_screenHeight);
			}
			return false;

		}
	}
	return true;
}

// -------------------------------- update item flow ---------------------------------------------

bool InputHandler::update_item(RenderItem* ri) {
	if (m_event.button.button == SDL_BUTTON_LEFT && m_event.type == SDL_MOUSEBUTTONDOWN) {
		m_dragEvent = true;
	}
	else if (m_event.button.button == SDL_BUTTON_LEFT && m_event.type == SDL_MOUSEBUTTONUP) {
		m_dragEvent = false;
	}
	else if (m_event.type == SDL_MOUSEMOTION) {
		m_mouseMotion.x = m_event.motion.xrel;
		m_mouseMotion.y = m_event.motion.yrel;
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
	return true;
}

void InputHandler::handle_move_event(RenderItem* ri) {
	glm::vec3 transform(0.0f, 0.0f, 0.0f);
	// Retrieve keyboard state
	if (m_keyState[SDL_SCANCODE_UP]) {
		transform.y += MOVESTEP;
	}
	if (m_keyState[SDL_SCANCODE_DOWN]) {
		transform.y -= MOVESTEP;
	}
	if (m_keyState[SDL_SCANCODE_RIGHT]) {
		transform.x += MOVESTEP;
	}
	if (m_keyState[SDL_SCANCODE_LEFT]) {
		transform.x -= MOVESTEP;
	}
	if (m_keyState[SDL_SCANCODE_LSHIFT]) {
		transform.z -= MOVESTEP;
	}
	if (m_keyState[SDL_SCANCODE_SPACE]) {
		transform.z += MOVESTEP;
	}
	// apply the transform to the render object
	if (transform.x != 0.0f || transform.y != 0.0f || transform.z != 0) {
		ri->translate(transform);
	}
}

void InputHandler::handle_scale_event(RenderItem* ri) {
	if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_EQUALS]) {
		ri->scale(glm::vec3(SCALESTEP, SCALESTEP, SCALESTEP));
	}
	if (m_keyState[SDL_SCANCODE_LALT] && m_keyState[SDL_SCANCODE_MINUS]) {
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
	if(rotation.x != 0.0f || rotation.y != 0.0f || rotation.z != 0.0f) {
		ri->rotate(rotation);
	}
}

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end update item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv

// -------------------------------- update camera flow -------------------------------------------

bool InputHandler::update_camera(Camera* camera, int sWidth, int sHeight) {
	m_keyState = SDL_GetKeyboardState(NULL);

	static float mX = sWidth / 2;
	static float mY = sHeight / 2;
	
	if (m_event.type == SDL_MOUSEMOTION) {
		mX += (m_event.motion.xrel * sensitivity);
		mY += (m_event.motion.yrel * sensitivity);
		camera->mouse_look(glm::vec2(mX , mY));
	}

	handle_camera_move(camera);

	return true;
}

void InputHandler::handle_camera_move(Camera* camera) {
	glm::vec3 translation(0.0f, 0.0f, 0.0f);

	if (m_keyState[SDL_SCANCODE_DOWN]) {
		camera->move_backward(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_UP]) {
		camera->move_forward(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_LEFT]) {
		camera->move_left(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_RIGHT]) {
		camera->move_right(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_LSHIFT]) {
		camera->move_down(MOVESTEP);
	}
	if (m_keyState[SDL_SCANCODE_SPACE]) {
		camera->move_up(MOVESTEP);
	}

	if (translation.x != 0.0f || translation.y != 0.0f || translation.z != 0.0f) {
		camera->translate(translation);
	}

}

// vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv end camera item vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv


