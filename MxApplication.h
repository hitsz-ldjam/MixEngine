#pragma once

#ifndef _MX_APPLICATION_H_
#define _MX_APPLICATION_H_

#include <iostream>
#include <vector>

#define SDL_MAIN_HANDLED
#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include "MxBehaviour.h"
#include "MxEvent.h"

namespace Mix {
    class MxApplication : public MxEvent {
    public:
        std::vector<std::shared_ptr<MxBehaviour>> behaviours;
        MxApplication(int argc = 0, char** argv = nullptr);
        ~MxApplication();
        int execute();
    private:
        bool quit;
        void onInitialize();
        void onUpdate();
        void onRender();
        void onCleanUp();
        void onKeyboardFocusGained(unsigned int timestamp, unsigned int windowID);
        void onKeyboardFocusLost(unsigned int timestamp, unsigned int windowID);
        void onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID);
        void onKeyUp(std::string scancode, std::string keycode, unsigned int timestamp, unsigned int windowID);
        void onMouseFocusGained(unsigned int timestamp, unsigned int windowID);
        void onMouseFocusLost(unsigned int timestamp, unsigned int windowID);
        void onMouseMotion(int x, int y, int btnBitmask, unsigned int timestamp, unsigned int windowID);
        void onMouseBtnDown(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID);
        void onMouseBtnUp(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID);
        void onMouseWheel(int x, int y, unsigned int timestamp, unsigned int windowID);
        void onQuit(unsigned int timestamp);
        void onUserEvent();
        void onWindowClose(unsigned int timestamp, unsigned int windowID);
        void onWindowExposed(unsigned int timestamp, unsigned int windowID);
    };
}

#endif
