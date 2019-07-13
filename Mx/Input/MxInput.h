#pragma once

#ifndef MX_INPUT_H_
#define MX_INPUT_H_

#include "../Math/MxVector2.h"
#include "../Engine/MxModuleBase.h"
#include <SDL2/SDL_events.h>

namespace Mix {

    /** 
     *  @note This class considers multiple events in a single event loop as one.\n
     *  On an AZERTY keyboard, pressing 'A' will emit a 'Q' scancode 
     *  (which represents the physical location) and an 'A' keycode.
     */
    class Input final {
    public:
        Input() = delete;
        Input(const Input&) = delete;

        // ----- Keyboard -----

        static bool AnyKey() noexcept { return mAnyKey; }

        static bool AnyKeyDown() noexcept { return mAnyKeyDown; }

        static bool GetAxisRaw(const SDL_Scancode _scancode) noexcept { return mKeyboardState[_scancode]; }

        static bool GetAxisRaw(const std::string& _name) noexcept { return GetAxisRaw(SDL_GetScancodeFromName(_name.c_str())); }

        static bool GetButton(const SDL_Scancode _scancode) noexcept { return mKeyboardEvent[_scancode] & mPressedMask; }

        static bool GetButton(const std::string& _name) noexcept { return GetButton(SDL_GetScancodeFromName(_name.c_str())); }

        static bool GetButtonDown(const SDL_Scancode _scancode) noexcept { return mKeyboardEvent[_scancode] & mDownMask; }

        static bool GetButtonDown(const std::string& _name) noexcept { return GetButtonDown(SDL_GetScancodeFromName(_name.c_str())); }

        static bool GetButtonUp(const SDL_Scancode _scancode) noexcept { return mKeyboardEvent[_scancode] & mUpMask; }

        static bool GetButtonUp(const std::string& _name) noexcept { return GetButtonUp(SDL_GetScancodeFromName(_name.c_str())); }

        static bool GetKey(const SDL_Keycode _keycode) noexcept { return GetButton(SDL_GetScancodeFromKey(_keycode)); }

        /** @note Redundant. Identical to @code Input::GetButton(const std::string&) @endcode . */
        static bool GetKey(const std::string& _name) noexcept { return GetKey(SDL_GetKeyFromName(_name.c_str())); }

        static bool GetKeyDown(const SDL_Keycode _keycode) noexcept { return GetButtonDown(SDL_GetScancodeFromKey(_keycode)); }

        /** @note Redundant. Identical to @code Input::GetButtonDown(const std::string&) @endcode . */
        static bool GetKeyDown(const std::string& _name) noexcept { return GetKeyDown(SDL_GetKeyFromName(_name.c_str())); }

        static bool GetKeyUp(const SDL_Keycode _keycode) noexcept { return GetButtonUp(SDL_GetScancodeFromKey(_keycode)); }

        /** @note Redundant. Identical to @code Input::GetButtonUp(const std::string&) @endcode . */
        static bool GetKeyUp(const std::string& _name) noexcept { return GetKeyUp(SDL_GetKeyFromName(_name.c_str())); }

        /** @note Directly calls @code SDL_GetModState() @endcode . */
        static auto GetKeyModState() noexcept { return SDL_GetModState(); }

        // ----- Mouse -----

        static auto MousePosition() noexcept {
            Math::Vector2i position;
            SDL_GetMouseState(&position.x, &position.y);
            return position;
        }

        static auto MousePositionDelta() noexcept { return mMousePosDelta; }

        /** 
         *  @return Button state. Can be tested using the @code SDL_BUTTON_MASK @endcode macros. 
         *  @note Directly calls @code SDL_GetMouseState(nullptr, nullptr) @endcode .
         */
        static auto MouseButtonState() noexcept { return SDL_GetMouseState(nullptr, nullptr); }

        /** @return Positive for upwards, negative for downwards. */
        static auto MouseScrollDelta() noexcept { return mMouseScrollDelta; }

        /** @param _button One of the @code SDL_BUTTON_ @endcode macros. */
        static bool GetMouseButtonDown(const Uint8 _button) noexcept { return mMouseButtonEvent[_button - 1] & mDownMask; }

        /** @param _button One of the @code SDL_BUTTON_ @endcode macros. */
        static bool GetMouseButtonUp(const Uint8 _button) noexcept { return mMouseButtonEvent[_button - 1] & mUpMask; }

        /** Please use @code Input::MouseButtonState() @endcode instead. */
        static bool GetMouseButton(const Uint8 _button) = delete;

        static void Awake();

        static void Process(const SDL_Event& _event);

        static void Reset();

    private:
        static const uint8_t mUpMask = 1,
                             mPressedMask = 1 << 1,
                             mDownMask = 1 << 2;

        static const Uint8* mKeyboardState;
        static uint8_t mKeyboardEvent[SDL_NUM_SCANCODES];
        static bool mAnyKey, mAnyKeyDown;

        static uint8_t mMouseButtonEvent[SDL_BUTTON_X2];
        static Math::Vector2i mMousePosDelta, mMouseScrollDelta;
    };
}

#endif
