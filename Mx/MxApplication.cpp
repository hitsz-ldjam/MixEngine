#include "MxApplication.h"

namespace Mix {
    Application::Application(int argc, char** argv) {
        // behaviours.reserve(capacity);
        quit = false;
        cleaned = false;
    }

    Application::~Application() {
        cleanup();
    }

    int Application::exec() {
        try {
            init();
            while (!quit) {
                Input::reset();
                SDL_Event event;
                while (SDL_PollEvent(&event)) {
                    process(event);
                }
                update();
                lateUpdate();
                render();
            }
            cleanup();
        }
        catch (const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return EXIT_SUCCESS;
    }

    void Application::cleanup() {
        if (!cleaned) {
            for (auto be : behaviours) 
                be->onDestroy();

            for (auto be : behaviours) {
                if (be)
                    delete be;
            }
            IMG_Quit();
            SDL_Quit();
        }
        cleaned = true;
    }

    void Application::init() {
        if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
            throw std::runtime_error("Error: Failed to initialize SDL2");

        if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
            throw std::runtime_error("Error: Failed to initialize SDL_image");

        // todo: initialize vulkan and other stuff

        quit = false;
        Input::initKeyboardState(SDL_GetKeyboardState(nullptr));

        for (auto be : behaviours) be->init();
    }

    void Application::process(SDL_Event& event) {

        // todo: handle and distribute events

        switch (event.type) {
        case SDL_KEYDOWN:
        {
            SDL_Scancode code = event.key.keysym.scancode;
            Input::anyKey = true;
            Input::keyEvent[code] |= Input::PressedMask;
            if (!event.key.repeat) {
                Input::anyKeyDown = true;
                Input::keyEvent[code] |= Input::FirstPressedMask;
            }
            break;
        }
        case SDL_KEYUP:
        {
            Input::keyEvent[event.key.keysym.scancode] |= Input::ReleasedMask;
            break;
        }
        case SDL_MOUSEBUTTONDOWN:
        {
            // if(event.button.clicks == 1)
            Input::mouseButtonEvent[event.button.button - 1] |= Input::FirstPressedMask;
            break;
        }
        case SDL_MOUSEBUTTONUP:
        {
            Input::mouseButtonEvent[event.button.button - 1] |= Input::ReleasedMask;
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
            for (auto be : behaviours)
                if (!be->onApplicationQuit())
                    quit = false;
            break;
        }
        default:
            break;
        }
    }

    void Application::render() {
        for (auto be : behaviours) be->onPreRender();

        // todo: call vulkan

        for (auto be : behaviours) be->onPostRender();
    }
}
