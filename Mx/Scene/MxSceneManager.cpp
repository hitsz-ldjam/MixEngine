#include "MxSceneManager.hpp"
#include "../../MixEngine.h"

namespace Mix {

    SceneManager* SceneManager::Get() {
        return MixEngine::Instance().getModule<SceneManager>();
    }

    std::shared_ptr<Scene> SceneManager::createScene(const std::string& _name) {
        if (mSceneNameIndexMap.count(_name)) {
            MX_LOG_ERROR("Attempting to create a new scene with name that is already been used.");
            return nullptr;
        }

        std::shared_ptr<Scene> scene = std::shared_ptr<Scene>(new Scene(_name, ++mNextId));
        scene->mThisPtr = scene;
        mIndexSceneMap[scene->getIndex()] = scene;
        mSceneNameIndexMap[_name] = scene->getIndex();
        return scene;
    }

    std::shared_ptr<Scene> SceneManager::getScene(const std::string& _name) {
        auto it = mSceneNameIndexMap.find(_name);
        if (it != mSceneNameIndexMap.end())
            return getScene(it->second);
        return nullptr;
    }

    std::shared_ptr<Scene> SceneManager::getScene(uint32_t _index) {
        auto it = mIndexSceneMap.find(_index);
        if (it != mIndexSceneMap.end())
            return it->second;
        return nullptr;
    }

    void SceneManager::loadScene(const std::string& _name) {
        auto it = mSceneNameIndexMap.find(_name);
        if (it != mSceneNameIndexMap.end()) {
            mIndexSceneMap[it->second]->load();
        }
        else
            MX_LOG_WARNING("Attempting to load Scene named %1% that doesn't exist.", _name);
    }

    void SceneManager::loadScene(uint32_t _index) {
        auto it = mIndexSceneMap.find(_index);
        if (it != mIndexSceneMap.end())
            it->second->load();
        else
            MX_LOG_WARNING("Attempting to load Scene by index with a wrong index.");
    }

    void SceneManager::setActiveScene(const std::shared_ptr<Scene>& _scene) {
        _scene->activate();
        if (mActiveScene)
            mActiveScene->inactivate();
        mActiveScene = _scene;
    }

    void SceneManager::unloadScene(const std::string& _name) {
        auto it = mSceneNameIndexMap.find(_name);
        if (it != mSceneNameIndexMap.end()) {
            mIndexSceneMap[it->second]->unload();
        }
        else
            MX_LOG_WARNING("Attempting to unload Scene named %1% that doesn't exist.", _name);
    }

    void SceneManager::unloadScene(uint32_t _index) {
        auto it = mIndexSceneMap.find(_index);
        if (it != mIndexSceneMap.end())
            it->second->unload();
        else
            MX_LOG_WARNING("Attempting to unload Scene by index with a wrong index.");
    }

    uint32_t SceneManager::getSceneCount() const {
        return mIndexSceneMap.size();
    }

    void SceneManager::load() {
    }

    void SceneManager::init() {
    }

    void SceneManager::sceneAwake() {
        mActiveScene->sceneAwake();
    }

    void SceneManager::sceneInit() {
        mActiveScene->sceneInit();
    }

    void SceneManager::sceneUpdate() {
        mActiveScene->sceneUpdate();
    }

    void SceneManager::sceneFixedUpdate() {
        mActiveScene->sceneFixedUpate();
    }

    void SceneManager::sceneLateUpdate() {
        mActiveScene->sceneLateUpate();
    }

    void SceneManager::scenePostRender() {
        mActiveScene->scenePostRender();
    }

    void SceneManager::registerGameObjectToMS(const HGameObject& _object) {
        mActiveScene->registerGameObject(_object);
    }

    /*SceneManager::SceneManager() : mActiveScene(new Scene(ScenesDebug::mainSceneName, ScenesDebug::mainSceneOnLoad, 0)) {
        mActiveScene->load();
        mScenes.push_back(mActiveScene);
    }

    SceneManager::~SceneManager() {
        for (auto s : mScenes) {
            s->unload();
            delete s;
        }
    }

    Scene* SceneManager::createScene(std::string _name, HierarchyLoader _onLoad) {
        auto s = new Scene(std::move(_name), std::move(_onLoad), mScenes.size());
        mScenes.push_back(s);
        return s;
    }

    Scene* SceneManager::loadScene(const std::string& _name) const {
        auto iter = std::find_if(mScenes.cbegin(), mScenes.cend(),
                                 [&name = _name](auto* _s) { return _s->name() == name; });
        if (iter == mScenes.cend())
            return nullptr;
        if (mActiveScene) mActiveScene->unload();
        (*iter)->load();
        return *iter;
    }

    Scene* SceneManager::loadScene(const size_t _buildIdx) const {
        if (_buildIdx >= mScenes.size())
            return nullptr;
        if (mActiveScene) mActiveScene->unload();
        auto s = mScenes[_buildIdx];
        s->load();
        return s;
    }*/
}
