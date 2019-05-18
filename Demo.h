#pragma once

#ifndef Demo_H_
#define DEMO_H_

#include <iostream>

#include "MixEngine.h"

inline void drawSlipLine() {
    std::cout << "------------------------" << std::endl;
}

inline std::ostream& operator<<(std::ostream& _os, const glm::vec3& _vec3) {
    return _os << "[ " << _vec3.x << ", " << _vec3.y << ", " << _vec3.z << " ]";
}

inline std::ostream& operator<<(std::ostream& _os, const Mix::Axis& _axis) {
    return _os << "{ " << _axis.x << ", " << _axis.y << ", " << _axis.z << " }";
}

inline std::ostream& operator<<(std::ostream& _os, const glm::quat& _quat) {
    return _os << "[ " << _quat.x << ", " << _quat.y << ", " << _quat.z << ", " << _quat.w << " ]";
}

inline std::ostream& operator<<(std::ostream& _os, const Mix::Transform& _trans) {
    _os << _trans.position() << std::endl;
    _os << _trans.localAxis() << std::endl;
    return _os << _trans.eulerAngle() << std::endl;
}

class Demo final : public Mix::Behaviour {
public:
    ~Demo() {
        delete clip;
        puts("Demo deleted");
    }

    void init() override {
        mWindow.create("Mix Engine Demo", 640, 480);

        SDL_Surface* surface = SDL_GetWindowSurface(mWindow.getWindowPtr());
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(mWindow.getWindowPtr());

        SDL_SetRelativeMouseMode(SDL_FALSE);

        clip = Mix::AudioClip::Open("TestResources/Media/test.mp3", // Pass audio path here
                                    Mix::AudioClipLoadType::DecompressOnLoad
                                    | Mix::AudioClipLoadType::ThreeD
                                    | Mix::AudioClipLoadType::Loop);
        clip->init(false);

        mVulkan.init();
        mVulkan.setTargetWindow(&mWindow);
        mVulkan.build();

        auto v = FindObjectOfType<Mix::Graphics::Vulkan>();

        mResources.init();
        gameObj = std::dynamic_pointer_cast<Mix::GameObject>(mResources.load("E:/Git/vulkan-learning-master/res/models/gltfSample/FlightHelmet/glTF/FlightHelmet.gltf"));

        camera = std::make_shared<Mix::GameObject>("Camera");
        camera->getComponent<Mix::Transform>()->position() = glm::vec3(0.0f, 0.0f, 3.0f);

        mTimer.Start();
    }

    void update() override {
        mTimer.Tick();
        auto tran = camera->getComponent<Mix::Transform>();
        tran->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_W))
            tran->translate(Mix::Axis::WorldForward, Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_S))
            tran->translate(-Mix::Axis::WorldForward, Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_D))
            tran->translate(Mix::Axis::WorldRight, Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_A))
            tran->translate(-Mix::Axis::WorldRight, Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_SPACE))
            tran->translate(Mix::Axis::WorldUp, Mix::Space::SELF);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_LCTRL))
            tran->translate(-Mix::Axis::WorldUp, Mix::Space::SELF);

        clip->set3DAttributes(&tran->position());
        std::cout << tran->forward() << std::endl;
        mVulkan.update(static_cast<float>(mTimer.DeltaTime()));
    }

private:
    Mix::Window mWindow;

    Mix::AudioClip* clip = nullptr;


    Mix::Graphics::Vulkan mVulkan;
    Mix::Timer              mTimer;

    std::shared_ptr<Mix::GameObject> camera;
    std::shared_ptr<Mix::GameObject> gameObj;
    Mix::Resource::Resources mResources;
};

#endif
