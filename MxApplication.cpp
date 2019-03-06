#include "MxApplication.h"

namespace Mix {
    Application::Application(int argc, char** argv) {
        _quit = false;
    }

    Application::~Application() {
        cleanup();
    }

    int Application::exec() {
        try {
            init();

            // todo: delete testcode
            Window window("SDL test", 600, 480);

            while(!_quit) {
                preProcess();
                SDL_Event event;
                while(SDL_PollEvent(&event)) {
                    process(event);
                }
                update();
                render();
            }
            cleanup();
        }
        catch(const std::exception& e) {
            std::cerr << e.what() << std::endl;
            return EXIT_FAILURE;
        }
        return 0;
    }

    void Application::init() {
        if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0)
            throw std::runtime_error("Error: Failed to initialize SDL2");

        if(IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0)
            throw std::runtime_error("Error: Failed to initialize SDL_image");

        // todo: initialize vulkan and other stuff
        
        _quit = false;
        Input::initKeyboardState(SDL_GetKeyboardState(nullptr));

        // todo: delete test code
        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void Application::preProcess() {
        Input::reset();
    }

    void Application::process(SDL_Event& event) {
        // todo: handle and distribute events
        switch(event.type) {
            case SDL_KEYDOWN:
            {
                SDL_Scancode code = event.key.keysym.scancode;
                Input::anyKey = true;
                Input::keyEvent[code] |= Input::PressedMask;
                if(!event.key.repeat) {
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
                // Use SDL_GetMouseState instead?
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
                quit();
                break;
            }
            default:
                break;
        }
    }

    void Application::update() {
        // todo: handle behaviors
        // todo: add test codes here

        bool i = Input::GetMouseButtonDown(SDL_BUTTON_LEFT); // Input::GetKeyDown(SDLK_f); // Input::GetAxisRaw(SDL_SCANCODE_F)
        if(i)
            std::cout << i << std::endl;
        
        glm::ivec2 d = Input::MousePositionDelta();
        if(d.y || d.x)
            std::cout << d.x << ", " << d.y << std::endl;
    }

    void Application::render() {
        // todo: call vulkan
    }

    void Application::quit() {
        // todo: handle behaviors
        _quit = true;
    }

    void Application::cleanup() {
        // todo: handle behaviors
        IMG_Quit();
        SDL_Quit();
    }
}
