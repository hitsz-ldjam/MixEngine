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
                SDL_Event _event;
                while(SDL_PollEvent(&_event)) {
                    process(_event);
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

        // todo: initialize Vulkan and other stuff
        
        _quit = false;
        Input::initState(SDL_GetKeyboardState(nullptr));
    }

    void Application::preProcess() {
        Input::resetKeyEvent();
    }

    void Application::process(SDL_Event& _event) {
        // todo: handel and distribute events
        switch(_event.type) {
            case SDL_KEYDOWN:
            {
                SDL_Scancode code = _event.key.keysym.scancode;
                Input::anyKey = true;
                Input::keyEvent[code] |= Input::KeyPressedMask;
                if(!_event.key.repeat) {
                    Input::anyKeyDown = true;
                    Input::keyEvent[code] |= Input::KeyFirstPressedMask;
                }
                break;
            }
            case SDL_KEYUP:
            {
                SDL_Scancode code = _event.key.keysym.scancode;
                Input::keyEvent[code] |= Input::KeyReleasedMask;
                break;
            }
            // todo: add other cases
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
        // todo: handling behaviors
        // add test codes here
        bool i = Input::GetKeyDown(SDLK_f); // Input::GetAxisRaw(SDL_SCANCODE_F)
        if(i)
            std::cout << i << std::endl;
    }

    void Application::render() {
        // todo: call vulkan
    }

    void Application::quit() {
        // todo: handling behaviors
        _quit = true;
    }

    void Application::cleanup() {
        // todo: handling behaviors
        IMG_Quit();
        SDL_Quit();
    }
}
