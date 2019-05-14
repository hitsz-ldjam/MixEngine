#include "MixEngine.h"

#include "Mx/Exceptions/MxExceptions.hpp"
#include "Mx/Input/MxInput.h"
#include "Mx/Time/MxTime.h"

#include <iostream>

namespace Mix {
    MixEngine::MixEngine(int _argc, char** _argv) {
        mQuit = false;
        mFmodCore = nullptr;
    }

    MixEngine::~MixEngine() {
        if(mFmodCore)
            mFmodCore->release();
        mFmodCore = nullptr;

        SDL_Quit();
    }

    int MixEngine::exec() {
        try {
            init();
            SDL_Event event;
            while(!mQuit) {
                while(SDL_PollEvent(&event))
                    process(event);
                update();
                lateUpdate();
                render();
            }
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void MixEngine::init() {
        mQuit = false;

        // initialize sdl
        if(SDL_Init(SDL_INIT_VIDEO))
            throw SdlInitializationError();

        // initialize fmod
        auto result = FMOD::System_Create(&mFmodCore);
        if(result != FMOD_OK)
            throw FmodInitializationError();

        result = mFmodCore->init(512, FMOD_INIT_NORMAL, nullptr);
        if(result != FMOD_OK)
            throw FmodInitializationError();

        Input::Init();

        // todo: replace with new timing interface
        start = lastFrame = std::chrono::high_resolution_clock::now();

        // todo: delete debug code
        mScene.init();
    }

    void MixEngine::process(const SDL_Event& _event) {
        switch(_event.type) {
            case SDL_KEYDOWN:
            {
                SDL_Scancode code = _event.key.keysym.scancode;
                Input::anyKey = true;
                Input::keyEvent[code] |= Input::PRESSED_MASK;
                if(!_event.key.repeat) {
                    Input::anyKeyDown = true;
                    Input::keyEvent[code] |= Input::FIRST_PRESSED_MASK;
                }
                break;
            }
            case SDL_KEYUP:
            {
                Input::keyEvent[_event.key.keysym.scancode] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                // if(event.button.clicks == 1)
                Input::mouseButtonEvent[_event.button.button - 1] |= Input::FIRST_PRESSED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                Input::mouseButtonEvent[_event.button.button - 1] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                break; // Use SDL_GetMouseState() for real-time info instead
            }
            case SDL_MOUSEWHEEL:
            {
                int deltaY = _event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
                deltaY *= _event.wheel.y;
                Input::mouseScrollDelta += glm::ivec2(_event.wheel.x, deltaY);
                break;
            }
            case SDL_QUIT:
            {
                mQuit = true;
                /*for(auto be : behaviours)
                    if(!be->onApplicationQuit())
                        mQuit = false;*/
                break;
            }
            default:
                break;
        }
    }

    void MixEngine::update() {
        // todo: delete debug code
        mScene.update();
    }

    void MixEngine::lateUpdate() {
        Input::Reset();

        // todo: replace with new timing interface
        Time::mTime = Time::getDuration(start);
        Time::mDeltaTime = Time::getDuration(lastFrame);
        lastFrame = std::chrono::high_resolution_clock::now();

        // update fmod
        mFmodCore->update();
    }

    // todo: call vulkan here
    void MixEngine::render() {}
}
