#pragma once

#ifndef _Demo_H_
#define _Demo_H_

#include <iostream>

#include "MixEngine.h"

void drawSlipLine() {
    std::cout << "------------------------" << std::endl;
}

std::ostream& operator<<(std::ostream& os, const glm::vec3& vec3) {
    return os << "[ " << vec3.x << ", " << vec3.y << ", " << vec3.z << " ]";
}

std::ostream& operator<<(std::ostream& os, const Mix::Axis& axis) {
    return os << "{ " << axis.x << ", " << axis.y << ", " << axis.z << " }";
}

std::ostream& operator<<(std::ostream& os, const glm::quat& quat) {
    return os << "[ " << quat.x << ", " << quat.y << ", " << quat.z << ", " << quat.w << " ]";
}

std::ostream& operator<<(std::ostream& os, const Mix::Transform& trans) {
    std::cout << trans.position() << std::endl;
    std::cout << trans.localAxis() << std::endl;
    return std::cout << trans.eulerAngle() << std::endl;
}

class Demo : public Mix::Behaviour {
public:
    ~Demo() {
        delete clip;
        delete obj;
        puts("Demo deleted");
    }

    virtual Component* copy() const {
        return new Demo();
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

        obj = new Mix::GameObject();
        obj->addChild(graphics.createModelObj(gltfLoader.loadFromGlb("E:/Git/vulkan-learning-master/res/models/gltfSample/DamagedHelmet/glTF-Binary/DamagedHelmet.glb", "DamagedHelmet")));
        timer.start();
    }

    void update() override {
        timer.tick();

        auto tran = obj->getComponent<Mix::Transform>();
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_W))
            tran->translate(Mix::Axis::worldForward, Mix::Space::World);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_S))
            tran->translate(-Mix::Axis::worldForward, Mix::Space::World);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_D))
            tran->translate(Mix::Axis::worldRight, Mix::Space::World);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_A))
            tran->translate(-Mix::Axis::worldRight, Mix::Space::World);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_SPACE))
            tran->translate(Mix::Axis::worldUp, Mix::Space::World);
        if (Mix::Input::GetAxisRaw(SDL_SCANCODE_LCTRL))
            tran->translate(-Mix::Axis::worldUp, Mix::Space::World);

        tran->lookAt(glm::vec3(0.0f, 0.0f, 0.0f));

        clip->set3DAttributes(&tran->position());

        static bool flag = true;

        graphics.update(static_cast<float>(timer.deltaTime()));
    }

private:
    Mix::Window window;

    Mix::AudioClip* clip = nullptr;
  
    glm::vec3 pos = {-10., 0., 3.}, lastPos = {0., 0., 0.}, vel = {0., 0., 0.};
  
    Mix::Graphics::Graphics graphics;
    Mix::Timer timer;

    Mix::GameObject* obj;
    Mix::Utils::GLTFLoader gltfLoader;

};

#endif
