#pragma once

#ifndef MAIN_SCENE_HPP_
#define MAIN_SCENE_HPP_

#include "../Mx/Scene/MxScene.h"
#include "../Mx/Graphics/MxGraphics.h"
#include "../Mx/Graphics/MxMaterial.h"
#include "../Mx/Graphics/Texture/MxTexture.h"
#include "../Mx/Resource/MxResourceLoader.h"
#include "../Mx/Resource/Model/MxModel.h"
#include "../Mx/Physics/MxPhysicsUtils.hpp"
#include "../Mx/Physics/MxRigidBodyUtils.hpp"
#include "../Mx/Component/RigidBody/MxRigidBody.h"
#include "../Mx/Component/Renderer/MxRenderer.h"
#include "../Mx/BuildIn/Camera/MxCamera.h"
#include "../MixEngine.h"
#include "../Scripts/Test.h"

// todo
namespace ScenesDebug {
    const std::string mainSceneName("Example Scene");

    const Mix::HierarchyLoader mainSceneOnLoad = []() {
        using namespace Mix;
        Hierarchy hierarchy;

        auto param = GetAdditionalParam<Texture2D>();
        param.mipLevel = 0;
        param.samplerInfo.minFilter = TextureFilterMode::Linear;
        param.samplerInfo.magFilter = TextureFilterMode::Linear;
        param.samplerInfo.mipFilter = TextureMipSampleMode::Linear;

        // Load player ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/player_ship.gltf");
            auto gameObject = model->genDefaultScene(GameObjectConInfo("PlayerShip"));
            gameObject->transform().translate(-2.0f, 0.0f, 0.0f, Space::World);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/player_ship_dif.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);
            hierarchy.insert(gameObject);
        }

        // Load enemy ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/enemy_ship.gltf");
            auto gameObject = model->genDefaultScene(GameObjectConInfo("EnemyShip"));
            gameObject->transform().translate(2.0f, 0.0f, 0.0f, Space::World);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/enemy_ship_diff.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);
            hierarchy.insert(gameObject);
        }

        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/Model/BoomBox/glTF/BoomBox.gltf");
            auto gameObject = model->genDefaultScene(GameObjectConInfo("TestBoomBox"));
            gameObject->transform().setLocalScale(Math::Vector3f(50.0f));

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/Model/BoomBox/glTF/BoomBox_baseColor.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            Physics::RigidBodyConstructionInfo info(10.0f, Physics::mixTransToBtTrans(gameObject->transform()), new btSphereShape(1));
            info.furtherSetup = [](btRigidBody::btRigidBodyConstructionInfo& _i) {
                _i.m_friction = .0f;
                _i.m_restitution = 1.f;
                _i.m_linearSleepingThreshold = .005f;
                _i.m_angularSleepingThreshold = .02f;
            };
            gameObject->addComponent<RigidBody>(info);

            hierarchy.insert(gameObject);
        }

        {
            auto gameObject = new GameObject("The floor");
            gameObject->transform().translate(0.0f, -8.0f, 0.0f);

            auto box = gameObject->addComponent<RigidBody>(.0f, gameObject->transform(), new btBoxShape({10.0f, 1.0f, 10.0f}));
            box->get().setFriction(.0f);
            box->get().setRestitution(1.f);
            box->get().setSleepingThresholds(.005f, .02f);

            hierarchy.insert(gameObject);
        }

        /*auto model1 = ResourceLoader::Get()->load<Model>("TestResources/Model/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf");
        auto object1 = model1->generateGameObject({ "Model1" });

        object1->transform().translate(3.0f, 0.0f, 0.0f, Space::WORLD);
        object1->transform().setLocalScale(Math::Vector3f(50.0f, 50.0f, 50.0f));*/

        auto camera = new GameObject("Camera");
        camera->transform().setPosition(Math::Vector3f(0.0f, 0.0f, -5.0f));
        camera->addComponent<Camera>(Window::Get()->extent());
        camera->addComponent<Scripts::TestScript>();
        hierarchy.insert(camera);

        return hierarchy;
    };
}

#endif
