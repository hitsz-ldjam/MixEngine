#include "MxInput.h"

namespace Mix {
    const Uint8* Input::keyboardState = nullptr;
    Uint8 Input::keyEvent[SDL_NUM_SCANCODES] = {0};
    bool Input::anyKey = false;
    bool Input::anyKeyDown = false;
    glm::ivec2 Input::mouseScrollDelta = {0, 0};
    Uint8 Input::mouseButtonEvent[SDL_BUTTON_X2] = {0};
}
