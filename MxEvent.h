#pragma once

#ifndef _MX_EVENT_H_
#define _MX_EVENT_H_

#include <string>

#include <SDL_events.h>

namespace Mix {
    class MxEvent {
    public:
        MxEvent() {};
        virtual ~MxEvent() {};
        virtual void onKeyboardFocusGained(unsigned int timestamp, unsigned int windowID) {};
        virtual void onKeyboardFocusLost(unsigned int timestamp, unsigned int windowID) {};
        /** @note On an AZERTY keyboard, pressing 'A' will emit a 'Q' scancode and an 'A' keycode. */
        virtual void onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID) {};
        virtual void onKeyUp(std::string scancode, std::string keycode, unsigned int timestamp, unsigned int windowID) {};
        virtual void onMouseFocusGained(unsigned int timestamp, unsigned int windowID) {};
        virtual void onMouseFocusLost(unsigned int timestamp, unsigned int windowID) {};
        // todo: add a switch for relative mouse mode
        /** @param x X coordinate, relative to window. If the mouse is in relative mode, it's the relative motion in the X direction.
            @param y Y coordinate, relative to window. If the mouse is in relative mode, it's the relative motion in the Y direction.
            @param btnBitmask Mouse button state bitmask. 1 for the left, 2 for the middle, 4 for the right.
        */
        virtual void onMouseMotion(int x, int y, int btnBitmask, unsigned int timestamp, unsigned int windowID) {};
        /** @param x X coordinate, relative to window.
            @param y Y coordinate, relative to window.
            @param btnIndex Index of the button that changed. 1 for the left, 2 for the middle, 4 for the right.
            @param clicks 1 for single click, etc.
        */
        virtual void onMouseBtnDown(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID) {};
        virtual void onMouseBtnUp(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID) {};
        /** @param x The amount scrolled horizontally, positive to the right and negative to the left.
            @param y the amount scrolled vertically, positive for down and negative for up.
        */
        virtual void onMouseWheel(int x, int y, unsigned int timestamp, unsigned int windowID) {};
        virtual void onQuit(unsigned int timestamp) {};

        // todo: add a class to handle user events
        virtual void onUserEvent() {};

        // test code
        virtual void onWindowClose(unsigned int timestamp, unsigned int windowID) {};
        virtual void onWindowExposed(unsigned int timestamp, unsigned int windowID) {};
    protected:
        virtual void onEvent(SDL_Event& _event) final;
    };
}

#endif
