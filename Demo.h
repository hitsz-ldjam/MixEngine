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
        delete mObj;
        puts("Demo deleted");
    }

    void init() override {
        mWindow.create("Mix Engine Demo", 640, 480);

        SDL_Surface* surface = SDL_GetWindowSurface(mWindow.getWindowPtr());
        SDL_FillRect(surface, nullptr, SDL_MapRGB(surface->format, 0, 128, 225));
        SDL_UpdateWindowSurface(mWindow.getWindowPtr());

        SDL_SetRelativeMouseMode(SDL_FALSE);

        clip = Mix::AudioClip::Open("Media/test.mp3", // Pass audio path here
                                    Mix::AudioClipLoadType::DecompressOnLoad
                                    | Mix::AudioClipLoadType::ThreeD
                                    | Mix::AudioClipLoadType::Loop);
        clip->Init(false);

        mGraphics.Init();
        mGraphics.SetTargetWindow(&mWindow);
        mGraphics.Build();

        mObj = new Mix::GameObject();
        mObj->AddChild(mGraphics.CreateModelObj(mGltfLoader.LoadFromGlb(
            ,
            "E:/Git/vulkan-learning-master/res/models/gltfSample/DamagedHelmet/glTF-Binary/DamagedHelmet.glb", "DamagedHelmet")));
        mTimer.Start();
    }

    void update() override {
        mTimer.Tick();

        auto tran = mObj->GetComponent<Mix::Transform>();
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_W))
            tran->translate(Mix::Axis::worldForward, Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_S))
            tran->translate(-Mix::Axis::worldForward, Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_D))
            tran->translate(Mix::Axis::worldRight, Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_A))
            tran->translate(-Mix::Axis::worldRight, Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_SPACE))
            tran->translate(Mix::Axis::worldUp, Mix::Space::WORLD);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_LCTRL))
            tran->translate(-Mix::Axis::worldUp, Mix::Space::WORLD);
        clip->Set3DAttributes(&tran->position());

        mGraphics.Update(static_cast<float>(mTimer.DeltaTime()));
    }

private:
    Mix::Window mWindow;

    Mix::AudioClip* clip = nullptr;


    Mix::Graphics::Vulkan mGraphics;
    Mix::Timer              mTimer;

    Mix::GameObject* mObj = nullptr;
    Mix::Utils::GltfLoader mGltfLoader;

};

#endif
