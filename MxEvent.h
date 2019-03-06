//todo: rebase

#pragma once

#ifndef _MX_EVENT_H_
#define _MX_EVENT_H_

#include <string>

#include <SDL2/SDL_events.h>

namespace Mix {
    class Event {
    public:
        Event() {};
        virtual ~Event() {};

        virtual void onKeyboardFocusGained(Uint32 timestamp, Uint32 windowID) {};
        virtual void onKeyboardFocusLost(Uint32 timestamp, Uint32 windowID) {};

        // Physical scancode key acting as keycode
        /** @note On an AZERTY keyboard, pressing 'A' will emit a 'Q' scancode (representing the physical location) and an 'A' keycode. */
        virtual void onKeyDown(std::string scancode, std::string keycode, bool isRepeat, Uint32 timestamp, Uint32 windowID) {};
        virtual void onKeyUp(std::string scancode, std::string keycode, Uint32 timestamp, Uint32 windowID) {};

        virtual void onMouseFocusGained(Uint32 timestamp, Uint32 windowID) {};
        virtual void onMouseFocusLost(Uint32 timestamp, Uint32 windowID) {};

        // todo: add a switch for relative mouse mode
        /** @param x X coordinate, relative to window. If the mouse is in relative mode, it's the relative motion in the X direction.
            @param y Y coordinate, relative to window. If the mouse is in relative mode, it's the relative motion in the Y direction.
            @param btnBitmask Mouse button state bitmask. 1 for the left, 2 for the middle, 4 for the right.
        */
        virtual void onMouseMotion(int x, int y, int btnBitmask, Uint32 timestamp, Uint32 windowID) {};

        /** @param x X coordinate, relative to window.
            @param y Y coordinate, relative to window.
            @param btnIndex Index of the button that changed. 1 for the left, 2 for the middle, 4 for the right.
            @param clicks 1 for single click, etc.
        */
        virtual void onMouseBtnDown(int x, int y, int btnIndex, unsigned char clicks, Uint32 timestamp, Uint32 windowID) {};
        virtual void onMouseBtnUp(int x, int y, int btnIndex, unsigned char clicks, Uint32 timestamp, Uint32 windowID) {};

        /** @param x The amount scrolled horizontally, positive to the right and negative to the left.
            @param y the amount scrolled vertically, positive for down and negative for up.
        */
        virtual void onMouseWheel(int x, int y, Uint32 timestamp, Uint32 windowID) {};

        virtual void onQuit(Uint32 timestamp) {};

        // todo: add a class to handle user events
        virtual void onUserEvent() {};

        // test code
        virtual void onWindowClose(Uint32 timestamp, Uint32 windowID) {};
        virtual void onWindowExposed(Uint32 timestamp, Uint32 windowID) {};
    };
}

#endif
