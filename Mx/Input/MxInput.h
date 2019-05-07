#pragma once

#ifndef _MX_INPUT_H
#define _MX_INPUT_H

#include <string>

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>

namespace Mix {

    /** @note This class considers multiple events in a single event loop as one.\n
     *  On an AZERTY keyboard, pressing 'A' will emit\n
     *  a 'Q' scancode (representing the physical location) and an 'A' keycode.
     */
    class Input {
        friend class MixEngine;

    public:
        // ----- Keyboard -----

        static bool AnyKey() {
            return anyKey;
        }

        static bool AnyKeyDown() {
            return anyKeyDown;
        }

        static bool GetAxisRaw(const SDL_Scancode _scancode) {
            return keyboardState[_scancode];
        }

        static bool GetAxisRaw(const std::string& _name) {
            return GetAxisRaw(SDL_GetScancodeFromName(_name.c_str()));
        }

        static bool GetButton(const SDL_Scancode _scancode) {
            return keyEvent[_scancode] & PRESSED_MASK;
        }

        static bool GetButton(const std::string& _name) {
            return GetButton(SDL_GetScancodeFromName(_name.c_str()));
        }

        static bool GetButtonDown(const SDL_Scancode _scancode) {
            return keyEvent[_scancode] & FIRST_PRESSED_MASK;
        }

        static bool GetButtonDown(const std::string& _name) {
            return GetButtonDown(SDL_GetScancodeFromName(_name.c_str()));
        }

        static bool GetButtonUp(const SDL_Scancode _scancode) {
            return keyEvent[_scancode] & RELEASED_MASK;
        }

        static bool GetButtonUp(const std::string& _name) {
            return GetButtonUp(SDL_GetScancodeFromName(_name.c_str()));
        }

        static bool GetKey(const SDL_Keycode _keycode) {
            return GetButton(SDL_GetScancodeFromKey(_keycode));
        }

        /** @note Redundant. Identical to @code Input::GetButton(const std::string&) @endcode . */
        static bool GetKey(const std::string& _name) {
            return GetKey(SDL_GetKeyFromName(_name.c_str()));
        }

        static bool GetKeyDown(const SDL_Keycode _keycode) {
            return GetButtonDown(SDL_GetScancodeFromKey(_keycode));
        }

        /** @note Redundant. Identical to @code Input::GetButtonDown(const std::string&) @endcode . */
        static bool GetKeyDown(const std::string& _name) {
            return GetKeyDown(SDL_GetKeyFromName(_name.c_str()));
        }

        static bool GetKeyUp(const SDL_Keycode _keycode) {
            return GetButtonUp(SDL_GetScancodeFromKey(_keycode));
        }

        /** @note Redundant. Identical to @code Input::GetButtonUp(const std::string&) @endcode . */
        static bool GetKeyUp(const std::string& _name) {
            return GetKeyUp(SDL_GetKeyFromName(_name.c_str()));
        }

        /** @note Directly calls @code SDL_GetModState() @endcode . */
        static SDL_Keymod GetKeyModState() {
            return SDL_GetModState();
        }

        // ----- Mouse -----

        /** @return The z axis indicates current button state, which can be tested using the @code SDL_BUTTON(X) @endcode macros. */
        static glm::ivec3 MousePosition() {
            glm::ivec3 position;
            position.z = SDL_GetMouseState(&position.x, &position.y);
            return position;
        }

        /** @return The z axis indicates current button state, which can be tested using the @code SDL_BUTTON(X) @endcode macros. */
        static glm::ivec3 MousePositionDelta() {
            glm::ivec3 delta;
            delta.z = SDL_GetRelativeMouseState(&delta.x, &delta.y);
            return delta;
        }

        /** @return Positive for upwards, negative for downwards. */
        static glm::ivec2 MouseScrollDelta() {
            return mouseScrollDelta;
        }

        /** @param _button One of the @code SDL_BUTTON(X) @endcode macros. */
        static bool GetMouseButtonDown(const uint8_t _button) {
            return mouseButtonEvent[static_cast<Uint8>(_button) - 1] & FIRST_PRESSED_MASK;
        }

        /** @param _button One of the @code SDL_BUTTON(X) @endcode macros. */
        static bool GetMouseButtonUp(const uint8_t _button) {
            return mouseButtonEvent[static_cast<Uint8>(_button) - 1] & RELEASED_MASK;
        }

    private:
        static const Uint8 FIRST_PRESSED_MASK = (1 << 2);
        static const Uint8 PRESSED_MASK = (1 << 1);
        static const Uint8 RELEASED_MASK = (1);

        static const Uint8* keyboardState;
        // FIRST_PRESSED | PRESSED | RELEASED
        static Uint8 keyEvent[SDL_NUM_SCANCODES];
        static bool anyKey;
        static bool anyKeyDown;

        static glm::ivec2 mouseScrollDelta;
        // FIRST_PRESSED | PRESSED | RELEASED
        static Uint8 mouseButtonEvent[SDL_BUTTON_X2];

        static void init() {
            keyboardState = SDL_GetKeyboardState(nullptr);
        }

        static void reset();
    };
}

#endif
