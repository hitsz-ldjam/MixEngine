#pragma once

#ifndef _Demo_H_
#define _Demo_H_

#include <iostream>

#include "MixEngine.h"

#include "MxVk\MxVkCore.h"

class Demo : public Mix::Behaviour {
public:
    ~Demo() {
        puts("Demo deleted");
    }

    void init() {
        window.create("Mix Engine Demo", 640, 480);

        SDL_Surface* surface = SDL_GetWindowSurface(window.getWindowPtr());
        SDL_FillRect(surface, NULL, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(window.getWindowPtr());

        Mix::Graphics::Core core;

        auto marker = [](const Mix::Graphics::PhysicalDeviceInfo& info)->uint32_t {
            if (info.properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
                return 100;
            else
                return 0;
        };

        core.setAppInfo("Demo", Mix::Version::makeVersion(0, 0, 1));
        core.setDebugMode(false);
        core.setTargetWindow(&window);

        core.createInstance();
        core.pickPhysicalDevice(marker);
        auto& devices = core.getAllPhysicalDeviceInfo();
        std::cout << "[Device name]: " << core.getPhysicalDeviceProperties().deviceName << std::endl;
        std::cout << std::boolalpha << core.getQueueFamilyIndices().present.has_value() << std::endl;

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
    }

private:
    Mix::Window window;
};

#endif
