#include "MxApplicationBase.h"
#include "MixEngine.h"
#include "Mx/Graphics/Texture/MxTexture.h"
#include "Mx/Resource/MxResourceLoader.h"
#include "Mx/Resource/Model/MxModel.h"
#include "Mx/Graphics/MxMaterial.h"
#include "Mx/Component/Renderer/MxRenderer.h"
#include "Mx/Graphics/MxGraphics.h"
#include "Mx/Scene/MxSceneManager.hpp"
#include "Mx/Graphics/Mesh/MxMeshUtils.h"
#include "Mx/Component/MeshFilter/MxMeshFilter.h"

#include "Scripts/PlayerShip.h"
#include "Scripts/EnemyShip.h"
#include "Scripts/ShipAdapter.h"
#include "Mx/BuildIn/Camera/MxCamera.h"

namespace Mix {

    ApplicationBase::ApplicationBase(const ApplicationSetting& _setting) :mSetting(_setting) {
    }

    void ApplicationBase::setFPSLimit(uint32_t _limit) {
        MixEngine::Instance().setFPSLimit(_limit);
    }

    void ApplicationBase::requestQuit() {
        MixEngine::Instance().requestQuit();
    }

    void ApplicationBase::startUp(std::vector<std::string> _args) {
    }

    void ApplicationBase::onModuleLoaded() {
    }

    void ApplicationBase::onMoudleInitialized() {
    }

    void ApplicationBase::onMainSceneCreated() {
        using namespace Mix;

        auto param = GetAdditionalParam<Texture2D>();
        param.mipLevel = 0;
        param.samplerInfo.minFilter = TextureFilterMode::Linear;
        param.samplerInfo.magFilter = TextureFilterMode::Linear;
        param.samplerInfo.mipFilter = TextureMipSampleMode::Linear;

        // Load player ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/player_ship.gltf");
            auto gameObject = model->genDefaultGameObjects("PlayerShip");

            gameObject->transform().setRotation(Quaternion::Identity);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/player_ship_dif.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            gameObject->addComponent<Scripts::PlayerShipScript>();
            gameObject->addComponent<Scripts::ShipAdapter>();

            //{
            //    auto collider = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/playerShip_collider.gltf");

            //    auto vertices = collider->getMeshes()[0]->getPositions();

            //    auto shape = new btConvexHullShape;
            //    for (auto& v : vertices)
            //        shape->addPoint({ v.x / 10, v.y / 10, v.z / 10 }, false);
            //    shape->recalcLocalAabb();

            //    auto proxy = new btCompoundShape;
            //    btTransform trans(btTransform::getIdentity());
            //    trans.setRotation({ 0, -3.14159, 0 });
            //    proxy->addChildShape(trans, shape);

            //    auto rb = gameObject->addComponent<RigidBody>(5.f,
            //                                                  gameObject->transform(),
            //                                                  proxy);
            //    //rb->setTrigger(true);
            //    rb->get().setFriction(.0f);
            //    rb->get().setRestitution(1.f);
            //    rb->get().setSleepingThresholds(.005f, .02f);
            //    rb->get().setLinearFactor({ 0, 1, 0 });
            //    rb->registerExitSlot([rb = rb](RigidBody* _other) {
            //        if (_other->getGameObject()->getName() == "The floor")
            //            if (rb->get().getLinearVelocity().y() < 12.522)
            //                rb->get().setLinearVelocity({ 0, 12.522, 0 });
            //                         });
            //}
        }

        // Load enemy ship
        {
            auto model = ResourceLoader::Get()->load<Model>("TestResources/SpaceShooter/Models/enemy_ship.gltf");
            auto gameObject = model->genDefaultGameObjects("EnemyShip");

            gameObject->transform().setRotation(Quaternion::Identity);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/enemy_ship_diff.png", &param);
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);

            gameObject->addComponent<Scripts::EnemyShipScript>();
            gameObject->addComponent<Scripts::ShipAdapter>();

            /*{
                auto sphere = gameObject->addComponent<RigidBody>(2.f,
                                                                  gameObject->transform(),
                                                                  new btSphereShape(1));
                sphere->get().setFriction(.0f);
                sphere->get().setRestitution(1.f);
                sphere->get().setSleepingThresholds(.005f, .02f);
                sphere->get().setLinearFactor({ 0, 1, 0 });
            }*/
        }

        {
            auto gameObject = GameObject::Create("Background");
            gameObject->transform().rotate(Vector3f::Right, Math::Radians(-90.0f), Space::World);
            gameObject->transform().translate(0.0f, -8.0f, 0.0f);

            auto[vertices, indices] = MeshUtils::Quad(Vector2f(30.0f, 60.0f));
            std::vector<Mesh::UV2DType> uvs = {
               {1.0f,1.0f},{0.0f,1.0f},{0.0f,0.0f},{1.0f,0.0f}
            };

            auto mesh = std::make_shared<Mesh>();
            mesh->setPositions(std::move(vertices));
            mesh->setIndices(std::move(indices), MeshTopology::Triangles_List, 0);
            mesh->setUVs(UVChannel::UV0, std::move(uvs));
            mesh->recalculateNormals();
            mesh->uploadMeshData(false);

            auto meshFilter = gameObject->addComponent<MeshFilter>();
            meshFilter->setMesh(mesh);

            auto diffuseTex = ResourceLoader::Get()->load<Texture2D>("TestResources/SpaceShooter/Textures/background.png");
            auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
            material->setTexture("diffuseTex", diffuseTex);

            auto renderer = gameObject->addComponent<Renderer>();
            renderer->setMaterial(material);
        }
    }

    void ApplicationBase::onAwake() {
    }

    void ApplicationBase::onInit() {
        auto mainCamaera = SceneManager::Get()->getActiveScene()->getMainCamera();
        mainCamaera->transform()->translate(Vector3f(0.0f, 0.0f, -20.0f));
        mainCamaera->transform()->lookAt(Vector3f::Zero);
    }

    void ApplicationBase::onUpdate() {
    }

    void ApplicationBase::onFixedUpdate() {
    }

    void ApplicationBase::onLateUpdate() {
    }

    void ApplicationBase::onRender() {
    }

    void ApplicationBase::onPostRender() {
    }

    bool ApplicationBase::onQuitRequested() {
        return true;
    }

    void ApplicationBase::onQuit() {
    }
}
