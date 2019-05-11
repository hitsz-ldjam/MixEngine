#include "MixEngine.h"

namespace Mix {
    MixEngine::MixEngine(int argc, char** argv) {
        quit = false;
    }

    MixEngine::~MixEngine() {
        SDL_Quit();
    }

    int MixEngine::exec() {
        try {
            init();
            SDL_Event event;
            while(!quit) {
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
        quit = false;

        if(SDL_Init(SDL_INIT_VIDEO))
            throw std::runtime_error("[ERROR] Failed to init SDL2");

        Input::Init();

        // todo: replace with new timing interface
        start = lastFrame = std::chrono::high_resolution_clock::now();

        // todo: delete debug code
        hierarchy.init();
    }

    void MixEngine::process(const SDL_Event& event) {
        switch(event.type) {
            case SDL_KEYDOWN:
            {
                SDL_Scancode code = event.key.keysym.scancode;
                Input::anyKey = true;
                Input::keyEvent[code] |= Input::PRESSED_MASK;
                if(!event.key.repeat) {
                    Input::anyKeyDown = true;
                    Input::keyEvent[code] |= Input::FIRST_PRESSED_MASK;
                }
                break;
            }
            case SDL_KEYUP:
            {
                Input::keyEvent[event.key.keysym.scancode] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONDOWN:
            {
                // if(event.button.clicks == 1)
                Input::mouseButtonEvent[event.button.button - 1] |= Input::FIRST_PRESSED_MASK;
                break;
            }
            case SDL_MOUSEBUTTONUP:
            {
                Input::mouseButtonEvent[event.button.button - 1] |= Input::RELEASED_MASK;
                break;
            }
            case SDL_MOUSEMOTION:
            {
                break; // Use SDL_GetMouseState() for real-time info instead
            }
            case SDL_MOUSEWHEEL:
            {
                int deltaY = event.wheel.direction == SDL_MOUSEWHEEL_NORMAL ? 1 : -1;
                deltaY *= event.wheel.y;
                Input::mouseScrollDelta += glm::ivec2(event.wheel.x, deltaY);
                break;
            }
            case SDL_QUIT:
            {
                quit = true;
                /*for(auto be : behaviours)
                    if(!be->onApplicationQuit())
                        quit = false;*/
                break;
            }
            default:
                break;
        }
    }

    void MixEngine::update() {
        // todo: delete debug code
        hierarchy.update();
    }

    void MixEngine::lateUpdate() {
        Input::Reset();

        // todo: replace with new timing interface
        Time::mTime = Time::getDuration(start);
        Time::mDeltaTime = Time::getDuration(lastFrame);
        lastFrame = std::chrono::high_resolution_clock::now();
    }

    // todo: call vulkan here
    void MixEngine::render() {}
}
