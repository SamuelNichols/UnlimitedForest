#pragma once

#include <camera/Camera.h>
#include <render_item/RenderItem.h>

#include <SDL2/SDL.h>

// const input vars
constexpr float MOVESTEP = 0.01f;
constexpr float SCALESTEP = 0.01f;
constexpr float sensitivity = 0.333f;

// selected item manager, really TODO type shit
enum SELECTED
{
	ITEM,
	CAMERA,
	END, //end of enum list to get total number of enums for iteration
};
// function to iterate enums
inline SELECTED& operator++(SELECTED& s) {
	s = static_cast<SELECTED>((static_cast<int>(s) + 1) % END);
	return s;
}

// Define the struct in the header, so it�s visible wherever InputHandler is used
struct SelectedItemTransform {
	float x = 0.0f;
	float y = 0.0f;
	float z = 0.0f;

	float rotx = 0.0f;
	float roty = 0.0f;
	float rotz = 0.0f;

	float scalex = 1.0f;
	float scaley = 1.0f;
	float scalez = 1.0f;
};

struct MouseMotion {
	int x = 0;
	int y = 0;
};

class InputHandler {
public:
	InputHandler();
	~InputHandler();

	// Match the function signature to what you actually need
	bool update(void);
	bool update_item(RenderItem* ri);
	bool update_camera(Camera* camera, int sWidth, int sHieght);

private:
	void handle_move_event(RenderItem* ri);
	void handle_scale_event(RenderItem* ri);
	void handle_drag_event(RenderItem* ri);
	void handle_rotate_event(RenderItem* ri);

	void handle_camera_move(Camera* cam);

	bool m_dragEvent;
	SDL_Event m_event;
	const Uint8* m_keyState;
	MouseMotion m_mouseMotion;
	SELECTED m_currItem;
};

