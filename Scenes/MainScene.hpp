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
            //gameObject->transform().translate(-5.0f, 0.0f, 0.0f, Space::World);
            gameObject->transform().rotate(-3.14159 / 2, 0, 0);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/player_ship_dif.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            {
                auto collider = ResourceLoader::Get()
                    ->load<Model>("TestResources/SpaceShooter/Models/playerShip_collider.gltf");

                auto vertices = collider->getMeshes()[0]->getPositions();

                auto shape = new btConvexHullShape;
                for(auto& v : vertices) {
                    float scale = 50.f;
                    shape->addPoint({v.x / scale, v.y / scale, v.z / scale}, false);
                }
                shape->recalcLocalAabb();

                auto proxy = new btCompoundShape;
                btTransform trans(btTransform::getIdentity());
                trans.setRotation({0, -3.14159, 0});
                proxy->addChildShape(trans, shape);

                auto rb = gameObject->addComponent<RigidBody>(5.f,
                                                              gameObject->transform(),
                                                              proxy);
                //rb->setTrigger(true);
                rb->get().setFriction(.0f);
                rb->get().setRestitution(1.f);
                rb->get().setSleepingThresholds(.005f, .02f);
                rb->get().setLinearFactor({0, 1, 0});
                rb->registerExitSlot([rb = rb](RigidBody* _other) {
                    if(_other->getGameObject()->getName() == "The floor")
                        if(rb->get().getLinearVelocity().y() < 12.522)
                            rb->get().setLinearVelocity({0, 12.522, 0});
                });
            }

            hierarchy.insert(gameObject);
        }

        // Load enemy ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/enemy_ship.gltf");
            auto gameObject = model->genDefaultScene(GameObjectConInfo("EnemyShip"));
            gameObject->transform().translate(0.0f, 15.0f, 0.0f, Space::World);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/enemy_ship_diff.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            auto sphere = gameObject->addComponent<RigidBody>(2.f,
                                                              gameObject->transform(),
                                                              new btSphereShape(1));
            sphere->get().setFriction(.0f);
            sphere->get().setRestitution(1.f);
            sphere->get().setSleepingThresholds(.005f, .02f);
            sphere->get().setLinearFactor({0, 1, 0});

            hierarchy.insert(gameObject);
        }

        {
            auto gameObject = new GameObject("The floor");
            gameObject->transform().translate(0.0f, -10.0f, 0.0f);

            auto box = gameObject->addComponent<RigidBody>(.0f, gameObject->transform(), new btBoxShape({10.0f, 1.0f, 10.0f}));
            box->get().setFriction(.0f);
            box->get().setRestitution(1.f);

            hierarchy.insert(gameObject);
        }

        {
            auto camera = new GameObject("Camera");
            camera->transform().setPosition(Math::Vector3f(0.0f, 0.0f, -5.0f));
            camera->addComponent<Camera>(Window::Get()->extent());
            camera->addComponent<Scripts::TestScript>();
            hierarchy.insert(camera);
        }

        {
            auto trigger = new GameObject("Quad");

            auto box = trigger->addComponent<RigidBody>(.0f,
                                                        trigger->transform(),
                                                        new btBoxShape({5.0f, .02f, 5.0f}));
            box->setTrigger(true);

            hierarchy.insert(trigger);
        }

        return hierarchy;
    };
}

#endif
