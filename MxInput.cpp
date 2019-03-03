#include "MxInput.h"

namespace Mix {
    bool Input::AnyKey() {
        return anyKey;
    }

    bool Input::AnyKeyDown() {
        return anyKeyDown;
    }

    bool Input::GetAxisRaw(const SDL_Scancode scancode) {
        return state[scancode];
    }

    bool Input::GetAxisRaw(const std::string& name) {
        return GetAxisRaw(SDL_GetScancodeFromName(name.c_str()));
    }

    bool Input::GetButton(const SDL_Scancode scancode) {
        return keyEvent[scancode] & KeyPressedMask;
    }

    bool Input::GetButton(const std::string& name) {
        return GetButton(SDL_GetScancodeFromName(name.c_str()));
    }

    bool Input::GetButtonDown(const SDL_Scancode scancode) {
        return keyEvent[scancode] & KeyFirstPressedMask;
    }

    bool Input::GetButtonDown(const std::string& name) {
        return GetButtonDown(SDL_GetScancodeFromName(name.c_str()));
    }

    bool Input::GetButtonUp(const SDL_Scancode scancode) {
        return keyEvent[scancode] & KeyReleasedMask;
    }

    bool Input::GetButtonUp(const std::string& name) {
        return GetButtonUp(SDL_GetScancodeFromName(name.c_str()));
    }

    bool Input::GetKey(const SDL_Keycode keycode) {
        return GetButton(SDL_GetScancodeFromKey(keycode));
    }

    bool Input::GetKey(const std::string& name) {
        return GetKey(SDL_GetKeyFromName(name.c_str()));
    }

    bool Input::GetKeyDown(const SDL_Keycode keycode) {
        return GetButtonDown(SDL_GetScancodeFromKey(keycode));
    }

    bool Input::GetKeyDown(const std::string& name) {
        return GetKeyDown(SDL_GetKeyFromName(name.c_str()));
    }

    bool Input::GetKeyUp(const SDL_Keycode keycode) {
        return GetButtonUp(SDL_GetScancodeFromKey(keycode));
    }

    bool Input::GetKeyUp(const std::string& name) {
        return GetKeyUp(SDL_GetKeyFromName(name.c_str()));
    }

    const Uint8* Input::state = nullptr;
    Uint8 Input::keyEvent[SDL_NUM_SCANCODES] = {0};
    bool Input::anyKey = false;
    bool Input::anyKeyDown = false;

    void Input::initState(const Uint8* keyboardState) {
        state = keyboardState;
    }

    void Input::resetKeyEvent() {
        memset(keyEvent, 0, SDL_NUM_SCANCODES * sizeof(*keyEvent));
        // std::fill(keyEvent, keyEvent + SDL_NUM_SCANCODES, 0);
        anyKey = false;
        anyKeyDown = false;
    }
}
