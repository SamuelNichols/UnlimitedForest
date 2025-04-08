#pragma once
#include <SDL.h>
#include <iostream>

namespace Core {

    // Define the struct in the header, so it’s visible wherever InputHandler is used
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
        bool update(SelectedItemTransform& offset);

    private:
        void handle_move_event(SelectedItemTransform& transform);
        void handle_drag_event(SelectedItemTransform& transform);
        void handle_rotate_event(SelectedItemTransform& transform);
        void scale(SelectedItemTransform& transform, float x, float y, float z);

        bool m_dragEvent;
        SDL_Event m_event;
        const Uint8* m_keyState;
        MouseMotion m_mouseMotion;

    };

}  // namespace Core
