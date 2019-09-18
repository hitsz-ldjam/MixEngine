#include "MxScene.h"
#include "../GameObject/MxGameObject.h"
#include "../Log/MxLog.h"
#include "../Component/Renderer/MxRenderer.h"
#include "../BuildIn/Camera/MxCamera.h"
#include "../Window/MxWindow.h"

namespace Mix {
    Scene::Scene(const std::string& _name, uint32_t _index)
        :mName(_name), mIndex(_index) {
    }

    std::shared_ptr<Scene> Scene::Create(const std::string& _name, uint32_t _index) {
        auto scene = std::shared_ptr<Scene>(new Scene(_name, _index));
        scene->mThisPtr = scene;
        return scene;
    }

    HGameObject Scene::findGameObject(const std::string& _name, bool _wholeScene) {
        /*for (auto& obj : mRootObjects) {
            if (obj.second->getName() == _name)
                return obj.second;
            if (_wholeScene) {
                auto child = obj.second->findChild(_name, true);
                if (child != nullptr)
                    return child;
            }
        }
        return nullptr;*/
        return findGameObjectIf([&](const HGameObject& _gameObject)->bool {return _name == _gameObject->getName(); });
    }

    void Scene::setFiller(const std::shared_ptr<SceneFiller>& _filler) {
        mFiller = _filler;
    }

    void Scene::registerCamera(const HCamera& _camera) {
        mRegisteredCamera.push_back(_camera);
    }

    void Scene::unregisterCamera(const HCamera& _camera) {

        auto found = std::find(mRegisteredCamera.begin(), mRegisteredCamera.end(), _camera);
        if (found != mRegisteredCamera.end())
            mRegisteredCamera.erase(found);

        if (mMainCamera == _camera) {
            mMainCamera = mRegisteredCamera.empty() ? HCamera() : mRegisteredCamera[0];
        }
    }

    void Scene::setMainCamera(const HCamera& _camera) {
        auto found = std::find(mRegisteredCamera.begin(), mRegisteredCamera.end(), _camera);
        if (found == mRegisteredCamera.end())
            mRegisteredCamera.push_back(_camera);
        mMainCamera = _camera;
    }

    HCamera Scene::getMainCamera() {
        if (mMainCamera == nullptr) {
            if (!mRegisteredCamera.empty())
                setMainCamera(mRegisteredCamera[0]);
            else
                MX_EXCEPT("No camera was registered in the scene.");
        }

        return mMainCamera;
    }

    Scene::~Scene() {
        if (mIsLoaded)
            unload();
    }

    void Scene::sceneAwake() {
        for (auto& gameObject : mNewRootObjects)
            mRootObjects[gameObject.getInstanceId()] = gameObject;
        mNewRootObjects.clear();

        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->awake();
        }
    }

    void Scene::sceneInit() {
        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->init();
        }

        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->setActive(true);
        }

        mNeedAwakeAndInit.clear();
    }

    void Scene::sceneUpdate() {
        for (auto& rootObject : mRootObjects) {
            if (rootObject.second->activeSelf())
                rootObject.second->update();
        }
    }

    void Scene::sceneFixedUpate() {
        for (auto rootObject : mRootObjects)
            rootObject.second->fixedUpdate();
    }

    void Scene::sceneLateUpate() {
        for (auto& rootObject : mRootObjects)
            rootObject.second->lateUpdate();
    }

    void Scene::scenePostRender() {
        flushNewAddedObject();
    }

    std::vector<HGameObject> Scene::getRootGameObjects() const {
        std::vector<HGameObject> results;
        results.reserve(mRootObjects.size());
        for (auto& pair : mRootObjects)
            results.push_back(pair.second);
        return results;
    }

    void Scene::registerGameObject(const HGameObject& _object) {
        if (_object.isDestroyed())
            return;
        if (_object->getParent() == nullptr) {
            mNewRootObjects.push_back(_object);
        }
        if (_object->activeInHierarchy()) {
            _object->setActive(false);
            mNeedAwakeAndInit.push_back(_object);
        }
        _object->setScene(mThisPtr.lock());
    }

    void Scene::unregisterGameObject(const HGameObject& _object) {
        if (_object->getScene().get() != this) {
            MX_LOG_WARNING("Attempting to unregister a GameObject from a scene which the GameObject doesn't belong to");
            return;
        }

        if (_object->getParent() == nullptr) {
            mRootObjects.erase(mRootObjects.find(_object.get()->getInstanceId()));
        }
    }

    void Scene::flushNewAddedObject() {
        for (auto& gameObject : mNewRootObjects)
            mRootObjects[gameObject.getInstanceId()] = gameObject;
        mNewRootObjects.clear();

        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->awake();
        }

        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->init();
        }

        for (auto& needAwakeObj : mNeedAwakeAndInit) {
            needAwakeObj->setActive(true);
        }

        mNeedAwakeAndInit.clear();
    }

    void Scene::load() {
        if (mIsLoaded)
            MX_EXCEPT("Attempting to load a Scene twice.");

        if (mFiller == nullptr) {
            MX_LOG_WARNING("Scene [%1%] has no specified SceneFiller. Use DefaultSceneFiller instead.", mName);

            SceneFiller* filler = new DefaultSceneFiller();
            filler->fill(mThisPtr.lock());
            delete filler;
        }
        else
            mFiller->fill(mThisPtr.lock());
        mIsLoaded = true;
    }

    void Scene::unload() {
        for (auto& gameObject : mRootObjects) {
            if (!gameObject.second.isDestroyed())
                gameObject.second->destroy();
        }
        mIsLoaded = false;
    }

    SceneRenderInfo Scene::_getRendererInfoPerFrame() {
        SceneRenderInfo info;

        // Set camera
        info.camera = mMainCamera.get().get();

        // Get all enabled Renderer
        for (auto& gameObject : mRootObjects) {
            FindRendererRecur(info, gameObject.second);
        }

        return info;
    }

    void Scene::FindRendererRecur(SceneRenderInfo& _info, const HGameObject& _object) {
        auto renderer = _object->getComponent<Renderer>();
        if (renderer != nullptr && renderer->getGameObject()->activeInHierarchy())
            _info.renderers.push_back(renderer.get().get());

        for (auto& child : _object->getAllChildren()) {
            FindRendererRecur(_info, child);
        }
    }

    HGameObject SceneFiller::createGameObject(const std::string& _name, const Tag& _tag, const LayerIndex _layerIndex, Flags<GameObjectFlags> _flags) const {
        auto gameObject = GameObject::CreateInternal(_name, _tag, _layerIndex, _flags, mTemp);
        return gameObject;
    }

    void SceneFiller::fill(const std::shared_ptr<Scene>& _scene) {
        mTemp = _scene;
        fillScene(mTemp);
        mTemp.reset();
    }

    void DefaultSceneFiller::fillScene(const std::shared_ptr<Scene>& _scene) {
        HGameObject gameObject = createGameObject("MainCamera");
        auto camera = gameObject->addComponent<Camera>(Window::Get()->getExtent());
        _scene->registerCamera(camera);
        _scene->setMainCamera(camera);
    }

}
