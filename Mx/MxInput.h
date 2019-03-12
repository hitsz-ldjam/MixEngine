#pragma once

#ifndef _MX_INPUT_H_
#define _MX_INPUT_H_

#include <string>
#include <vector>

#include <glm/vec2.hpp>

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

#include "MxApplication.h"

namespace Mix {

    /** @note This class considers several events in a loop as one.*/
    class Input {
        friend class Application;

    public:
        static bool AnyKey() {
            return anyKey;
        }

        static bool AnyKeyDown() {
            return anyKeyDown;
        }

        static bool GetAxisRaw(const SDL_Scancode scancode) {
            return keyboardState[scancode];
        }

        static bool GetAxisRaw(const std::string& name) {
            return GetAxisRaw(SDL_GetScancodeFromName(name.c_str()));
        }

        // On an AZERTY keyboard, pressing 'A' will emit a 'Q' scancode (representing the physical location) and an 'A' keycode.

        static bool GetButton(const SDL_Scancode scancode) {
            return keyEvent[scancode] & PressedMask;
        }

        static bool GetButton(const std::string& name) {
            return GetButton(SDL_GetScancodeFromName(name.c_str()));
        }

        static bool GetButtonDown(const SDL_Scancode scancode) {
            return keyEvent[scancode] & FirstPressedMask;
        }

        static bool GetButtonDown(const std::string& name) {
            return GetButtonDown(SDL_GetScancodeFromName(name.c_str()));
        }

        static bool GetButtonUp(const SDL_Scancode scancode) {
            return keyEvent[scancode] & ReleasedMask;
        }

        static bool GetButtonUp(const std::string& name) {
            return GetButtonUp(SDL_GetScancodeFromName(name.c_str()));
        }

        static bool GetKey(const SDL_Keycode keycode) {
            return GetButton(SDL_GetScancodeFromKey(keycode));
        }

        // Redundant method. The same as GetButton(const std::string& name).
        static bool GetKey(const std::string& name) {
            return GetKey(SDL_GetKeyFromName(name.c_str()));
        }

        static bool GetKeyDown(const SDL_Keycode keycode) {
            return GetButtonDown(SDL_GetScancodeFromKey(keycode));
        }

        // Redundant method. The same as GetButtonDown(const std::string& name).
        static bool GetKeyDown(const std::string& name) {
            return GetKeyDown(SDL_GetKeyFromName(name.c_str()));
        }

        /** @note Directly calls SDL_GetModState(). */
        static SDL_Keymod GetKeyModState() {
            return SDL_GetModState();
        }

        static bool GetKeyUp(const SDL_Keycode keycode) {
            return GetButtonUp(SDL_GetScancodeFromKey(keycode));
        }

        // Redundant method. The same as GetButtonUp(const std::string& name).
        static bool GetKeyUp(const std::string& name) {
            return GetKeyUp(SDL_GetKeyFromName(name.c_str()));
        }

        /** @param btnBitmask Current button state, which can be tested using the SDL_BUTTON(X) macros. */
        static glm::ivec2 MousePosition(Uint32* btnBitmask = nullptr) {
            glm::ivec2 position;
            Uint32 state = SDL_GetMouseState(&position.x, &position.y);
            if(btnBitmask)
                *btnBitmask = state;
            return position;
        }

        /** @param btnBitmask Current button state, which can be tested using the SDL_BUTTON(X) macros. */
        static glm::ivec2 MousePositionDelta(Uint32* btnBitmask = nullptr) {
            glm::ivec2 delta;
            Uint32 state = SDL_GetRelativeMouseState(&delta.x, &delta.y);
            if(btnBitmask)
                *btnBitmask = state;
            return delta;
        }

        /** @note positive (up) or negative (down) */
        static glm::ivec2 MouseScrollDelta() {
            return mouseScrollDelta;
        }

        /** @param button May be one of the SDL_BUTTON macros */
        static bool GetMouseButtonDown(const Uint8 button) {
            return mouseButtonEvent[button - 1] & FirstPressedMask;
        }

        /** @param button May be one of the SDL_BUTTON macros */
        static bool GetMouseButtonUp(const Uint8 button) {
            return mouseButtonEvent[button - 1] & ReleasedMask;
        }

    private:
        static const Uint8 FirstPressedMask = (1 << 2);
        static const Uint8 PressedMask = (1 << 1);
        static const Uint8 ReleasedMask = (1);

        static const Uint8* keyboardState;
        // first pressed | pressed | released
        static Uint8 keyEvent[SDL_NUM_SCANCODES];
        static bool anyKey;
        static bool anyKeyDown;

        static glm::ivec2 mouseScrollDelta;
        static Uint8 mouseButtonEvent[SDL_BUTTON_X2];
        
        static void initKeyboardState(const Uint8* keyboardState) {
            Input::keyboardState = keyboardState;
        }

        static void reset() {
            memset(keyEvent, 0, SDL_NUM_SCANCODES * sizeof(*keyEvent));
            anyKey = false;
            anyKeyDown = false;
            mouseScrollDelta = {0, 0};
            memset(mouseButtonEvent, 0, SDL_BUTTON_X2 * sizeof(*mouseButtonEvent));
        }
    };
}

#endif
