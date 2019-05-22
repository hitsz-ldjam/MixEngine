#pragma once

#ifndef MX_INPUT_H_
#define MX_INPUT_H_

#include <SDL2/SDL_keyboard.h>
#include <SDL2/SDL_mouse.h>
#include <glm/vec2.hpp>

#include <string>

namespace Mix {

    /** 
     *  @note This class considers multiple events in a single event loop as one.\n
     *  On an AZERTY keyboard, pressing 'A' will emit a 'Q' scancode 
     *  (which represents the physical location) and an 'A' keycode.
     */
    class Input final {
        friend class MixEngine;

    public:
        Input() = delete;

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

        static glm::ivec2 MousePosition() {
            glm::ivec2 position;
            SDL_GetMouseState(&position.x, &position.y);
            return position;
        }

        static glm::ivec2 MousePositionDelta() {
            glm::ivec2 delta;
            SDL_GetRelativeMouseState(&delta.x, &delta.y);
            return delta;
        }

        /** @return Button state. Can be tested using the @code SDL_BUTTON_MASK @endcode macros. */
        static Uint32 MouseButtonState() {
            return SDL_GetMouseState(nullptr, nullptr);
        }

        /** @return Positive for upwards, negative for downwards. */
        static glm::ivec2 MouseScrollDelta() {
            return mouseScrollDelta;
        }

        /** @param _button One of the @code SDL_BUTTON_ @endcode macros. */
        static bool GetMouseButtonDown(const Uint8 _button) {
            return mouseButtonEvent[_button - 1] & FIRST_PRESSED_MASK;
        }

        /** @param _button One of the @code SDL_BUTTON_ @endcode macros. */
        static bool GetMouseButtonUp(const Uint8 _button) {
            return mouseButtonEvent[_button - 1] & RELEASED_MASK;
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

        static void Init();

        static void Reset();
    };
}

#endif
