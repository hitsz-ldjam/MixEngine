#include "MxApplication.h"

namespace Mix {
    Application::Application(int argc, char** argv) {
        // behaviours.reserve(capacity);
        quit = false;
    }

    Application::~Application() {
        for(auto be : behaviours) delete be;
        behaviours.clear();
        IMG_Quit();
        SDL_Quit();
    }

    int Application::exec() {
        try {
            init();
            while(!quit) {
                preEvent();
                while(SDL_PollEvent(&event)) {
                    process(event);
                }
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

    void Application::init() {
        if(SDL_Init(SDL_INIT_VIDEO) != 0)
            throw std::runtime_error("Error: Failed to initialize SDL2");

        if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
            throw std::runtime_error("Error: Failed to initialize SDL_image");

        // todo: initialize vulkan and other stuff

        AudioManager::getInstance();
        Input::keyboardState = SDL_GetKeyboardState(nullptr);

        quit = false;
        start = lastFrame = std::chrono::high_resolution_clock::now();

        for(auto be : behaviours) be->init();
    }

    void Application::lateUpdate() {
        for(auto be : behaviours) be->lateUpdate();
        AudioManager::getInstance().update();
    }

    void Application::process(SDL_Event& event) {

        // todo: handle and distribute events

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
                // Use SDL_GetMouseState instead (?)
                break;
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
                for(auto be : behaviours)
                    if(!be->onApplicationQuit())
                        quit = false;
                break;
            }
            default:
                break;
        }
    }

    void Application::render() {
        preRender();
        // todo: call vulkan
        postRender();
    }

    void Application::postRender() {
        for(auto be : behaviours) be->onPostRender();
    }

    void Application::preEvent() {
        Time::time = Time::getDuration(start);
        Time::deltaTime = Time::getDuration(lastFrame);
        lastFrame = std::chrono::high_resolution_clock::now();
        Input::reset();
    }

    void Application::preRender() {
        for(auto be : behaviours) be->onPreRender();
    }
}
