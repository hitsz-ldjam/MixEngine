#include "MxScene.h"
#include "../GameObject/MxGameObject.h"
#include "../Resource/Model/MxModel.h"
#include "../Resource/MxResourceLoader.h"
#include "../BuildIn/Camera/MxCamera.h"
#include "../Window/MxWindow.h"
#include "../../Scripts/Test.h"
#include "../Graphics/Texture/MxTexture.h"
#include "../Graphics/MxMaterial.h"
#include "../Component/Renderer/MxRenderer.h"
#include "../Graphics/MxGraphics.h"
#include "../Component/RigidBody/MxRigidBody.h"
#include "../Physics/MxPhysicsUtils.hpp"
#include "../Physics/MxRigidBodyUtils.hpp"
#include <fstream>

namespace Mix {
    Scene::Scene(std::string _name, const size_t _buildIdx) : mName(std::move(_name)),
        mBuildIndex(_buildIdx),
        mIsLoaded(false) {
// todo: manage Scene by SceneMgr
        mIsLoaded = true;

    }

    void Scene::awake() {
        auto param = GetAdditionalParam<Texture2D>();
        param.mipLevel = 0;
        param.samplerInfo.minFilter = TextureFilterMode::Linear;
        param.samplerInfo.magFilter = TextureFilterMode::Linear;
        param.samplerInfo.mipFilter = TextureMipSampleMode::Linear;

        // Load player ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/player_ship.gltf");
            auto gameObject = model->genDefaultScene({ "PlayerShip" });
            gameObject->transform().translate(-2.0f, 0.0f, 0.0f, Space::World);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/player_ship_dif.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);
            addGameObject(gameObject);
        }

        // Load enemy ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/enemy_ship.gltf");
            auto gameObject = model->genDefaultScene({ "EnemyShip" });
            gameObject->transform().translate(2.0f, 0.0f, 0.0f, Space::World);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/enemy_ship_diff.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);
            addGameObject(gameObject);
        }

        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/Model/BoomBox/glTF/BoomBox.gltf");
            auto gameObject = model->genDefaultScene({ "TestBoomBox" });
            gameObject->transform().setLocalScale(Math::Vector3f(50.0f));

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/Model/BoomBox/glTF/BoomBox_baseColor.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            Physics::RigidBodyConstructionInfo info(10.0f, Physics::mixTransformToBtTransform(gameObject->transform()), new btSphereShape(1));
            info.furtherSetup = [](btRigidBody::btRigidBodyConstructionInfo& _i) {
                _i.m_additionalDamping = false;
                _i.m_restitution = 1.0f;
                _i.m_angularDamping = 0.0f;
                _i.m_linearDamping = 0.0f;
                _i.m_rollingFriction = 0.0f;
                _i.m_friction = 0.0f;
            };

            auto box = gameObject->addComponent<RigidBody>(info);
            addGameObject(gameObject);
        }

        {
            auto gameObject = new GameObject({ "Fucking floor" });
            gameObject->transform().translate(0.0f, -8.0f, 0.0f);

            Physics::RigidBodyConstructionInfo info(0.0f, Physics::mixTransformToBtTransform(gameObject->transform()), new btBoxShape({ 10.0f,1.0f,10.0f }));
            info.furtherSetup = [](btRigidBody::btRigidBodyConstructionInfo& _i) {
                _i.m_additionalDamping = false;
                _i.m_restitution = 1.0f;
                _i.m_angularDamping = 0.0f;
                _i.m_linearDamping = 0.0f;
                _i.m_rollingFriction = 0.0f;
                _i.m_friction = 0.0f;
            };

            auto box = gameObject->addComponent<RigidBody>(info);
            box->get().setRestitution(1.0f);
            box->get().setFriction(0.0f);
            box->get().setDamping(0.0f, 0.0f);
            addGameObject(gameObject);
        }

        /*auto model1 = ResourceLoader::Get()->load<Model>("TestResources/Model/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf");
        auto object1 = model1->generateGameObject({ "Model1" });

        object1->transform().translate(3.0f, 0.0f, 0.0f, Space::WORLD);
        object1->transform().setLocalScale(Math::Vector3f(50.0f, 50.0f, 50.0f));*/

        auto camera = new GameObject({ "Camera" });
        camera->transform().setPosition(Math::Vector3f(0.0f, 0.0f, -5.0f));
        camera->addComponent<Camera>(Window::Get()->extent());
        camera->addComponent<Scripts::TestScript>();


        // addGameObject(object1);
        addGameObject(camera);

        for (auto obj : mHierarchy)
            obj->awake();
    }

    void Scene::init() {
        for (auto obj : mHierarchy)
            obj->init();
    }

    void Scene::update() {
        for (auto obj : mHierarchy)
            obj->update();
    }

    void Scene::fixedUpate() {
        for (auto obj : mHierarchy)
            obj->fixedUpdate();
    }

    void Scene::lateUpate() {
        for (auto obj : mHierarchy)
            obj->lateUpdate();
    }
}
