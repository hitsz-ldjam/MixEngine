#include "MxApplication.h"

namespace Mix {
    MxApplication::MxApplication(int argc, char** argv) {
        // behaviours.reserve(__size);
        quit = false;
    }

    MxApplication::~MxApplication() {
        onCleanUp();
    }

    int MxApplication::execute() {
        try {
            onInitialize();
            while(!quit) {
                SDL_Event _event;
                while(SDL_PollEvent(&_event)) {
                    onEvent(_event);
                }
                onUpdate();
                onRender();
            }
            onCleanUp();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return 0;
    }

    void MxApplication::onInitialize() {
        // Initialize SDL2
        if(SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO) != 0)
            throw std::runtime_error("Error: Failed to initialize SDL2");

        if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
            throw std::runtime_error("Error: Failed to initialize SDL_image");

        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096) != 0)
            throw std::runtime_error("Error: Failed to initialize SDL_Mixer");

        for(const auto& behaviour : behaviours)
            behaviour->init();
    }

    void MxApplication::onUpdate() {
        for(const auto& behaviour : behaviours)
            behaviour->update();
    }

    void MxApplication::onRender() {
        return;
    }

    void MxApplication::onCleanUp() {
        behaviours.clear();
        Mix_Quit();
        IMG_Quit();
        SDL_Quit();
    }

    void MxApplication::onKeyboardFocusGained(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onKeyboardFocusGained(timestamp, windowID);
    }

    void MxApplication::onKeyboardFocusLost(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onKeyboardFocusLost(timestamp, windowID);
    }

    void MxApplication::onKeyDown(std::string scancode, std::string keycode, bool isRepeat, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onKeyDown(scancode, keycode, isRepeat, timestamp, windowID);
    }

    void MxApplication::onKeyUp(std::string scancode, std::string keycode, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onKeyUp(scancode, keycode, timestamp, windowID);
    }

    void MxApplication::onMouseFocusGained(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseFocusGained(timestamp, windowID);
    }

    void MxApplication::onMouseFocusLost(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseFocusLost(timestamp, windowID);
    }

    void MxApplication::onMouseMotion(int x, int y, int btnBitmask, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseMotion(x, y, btnBitmask, timestamp, windowID);
    }

    void MxApplication::onMouseBtnDown(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseBtnDown(x, y, btnIndex, clicks, timestamp, windowID);
    }

    void MxApplication::onMouseBtnUp(int x, int y, int btnIndex, unsigned char clicks, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseBtnUp(x, y, btnIndex, clicks, timestamp, windowID);
    }

    void MxApplication::onMouseWheel(int x, int y, unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onMouseWheel(x, y, timestamp, windowID);
    }

    void MxApplication::onQuit(unsigned int timestamp) {
        for(const auto& behaviour : behaviours)
            behaviour->onQuit(timestamp);
        quit = true;
    }

    void MxApplication::onUserEvent() {
        for(const auto& behaviour : behaviours)
            behaviour->onUserEvent();
    }

    void MxApplication::onWindowClose(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onWindowClose(timestamp, windowID);
    }

    void MxApplication::onWindowExposed(unsigned int timestamp, unsigned int windowID) {
        for(const auto& behaviour : behaviours)
            behaviour->onWindowExposed(timestamp, windowID);
    }
}
