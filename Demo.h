#pragma once

#ifndef _Demo_H_
#define _Demo_H_

#include <iostream>

#include "MixEngine.h"

#include "MxVk/MxVkGraphics.h"

class Demo : public Mix::Behaviour {
public:
    ~Demo() {
        delete clip;
        puts("Demo deleted");
    }

    void init() override {
        window.create("Mix Engine Demo", 640, 480);

        SDL_Surface* surface = SDL_GetWindowSurface(window.getWindowPtr());
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(window.getWindowPtr());

        SDL_SetRelativeMouseMode(SDL_FALSE);

        clip = Mix::AudioClip::open("", // Pass audio path here
                                    Mix::AudioClipLoadType::DecompressOnLoad
                                    | Mix::AudioClipLoadType::ThreeD
                                    | Mix::AudioClipLoadType::Loop);
        clip->init(false);
        clip->set3DAttributes(&pos, &vel);
        clip->play();
        lastPos = pos;

        graphics.init();
        graphics.setTargetWindow(&window);
        graphics.build();
    }

    void update() override {
        glm::ivec2 d = Mix::Input::MousePositionDelta();
        if(d.y || d.x)
            std::cout << d.x << ", " << d.y << std::endl;

        pos.x = sin(Mix::Time::getTime() * 0.5) * 10;
        vel.x = (pos.x - lastPos.x) / Mix::Time::getDeltaTime();
        lastPos = pos;
        clip->set3DAttributes(&pos, &vel);
        
        graphics.update(10 * Mix::Time::getDeltaTime());
    }

private:
    Mix::Window window;

    Mix::AudioClip* clip = nullptr;
    glm::vec3 pos = {-10., 0., 3.}, lastPos = {0., 0., 0.}, vel = {0., 0., 0.};

    Mix::Graphics::Graphics graphics;
};

#endif
