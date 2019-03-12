#pragma once

#ifndef _Demo_H_
#define _Demo_H_

#include <iostream>

#include "MixEngine.h"

#include "MxVk/MxVkGraphics.h"

class Demo : public Mix::Behaviour {
public:
    virtual ~Demo() {
        puts("Demo deleted");
    }

    void init() {
        window.create("Mix Engine Demo", 640, 480);

        SDL_Surface* surface = SDL_GetWindowSurface(window.getWindowPtr());
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(window.getWindowPtr());

        graphics.init();
        graphics.setTargetWindow(&window);
        graphics.build();

        SDL_SetRelativeMouseMode(SDL_FALSE);
    }

    void update() {
        bool i = Mix::Input::GetKeyDown(SDLK_f);
        // Mix::Input::GetMouseButtonDown(SDL_BUTTON_LEFT);
        // Mix::Input::GetAxisRaw(SDL_SCANCODE_F);
        if (i)
            std::cout << i << std::endl;

        glm::ivec2 d = Mix::Input::MousePositionDelta();
        if (d.y || d.x)
            std::cout << d.x << ", " << d.y << std::endl;

        graphics.update(0.1f);
    }

private:
    Mix::Window window;

    Mix::Graphics::Graphics graphics;
};

#endif
