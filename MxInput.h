#pragma once

#ifndef _MX_INPUT_H_
#define _MX_INPUT_H_

#include <SDL_keyboard.h>

#include "MxApplication.h"

namespace Mix {
    class Input {
        friend class Application;
    public:
        static bool AnyKey();
        static bool AnyKeyDown();
        static bool GetAxisRaw(const SDL_Scancode scancode);
        static bool GetAxisRaw(const std::string& name);
        static bool GetButton(const SDL_Scancode scancode);
        static bool GetButton(const std::string& name);
        static bool GetButtonDown(const SDL_Scancode scancode);
        static bool GetButtonDown(const std::string& name);
        static bool GetButtonUp(const SDL_Scancode scancode);
        static bool GetButtonUp(const std::string& name);
        static bool GetKey(const SDL_Keycode keycode);
        // Redundant method. The same as GetButton.
        static bool GetKey(const std::string& name);
        static bool GetKeyDown(const SDL_Keycode keycode);
        // Redundant method. The same as GetButtonDown.
        static bool GetKeyDown(const std::string& name);
        static bool GetKeyUp(const SDL_Keycode keycode);
        // Redundant method. The same as GetButtonUp.
        static bool GetKeyUp(const std::string& name);
    private:
        static const Uint8 KeyFirstPressedMask = (1 << 2);
        static const Uint8 KeyPressedMask = (1 << 1);
        static const Uint8 KeyReleasedMask = (1);
        static const Uint8* state;
        // first pressed | pressed | released
        static Uint8 keyEvent[SDL_NUM_SCANCODES];
        static bool anyKey;
        static bool anyKeyDown;
        static void initState(const Uint8* keyboardState);
        static void resetKeyEvent();
    };
}

#endif
