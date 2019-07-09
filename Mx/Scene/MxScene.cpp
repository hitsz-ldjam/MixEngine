#include "MxScene.h"
#include "../GameObject/MxGameObject.h"
#include "../Resource/Model/MxModel.h"
#include "../../MixEngine.h"
#include "../Resource/MxResourceLoader.h"
#include "../Preset/Camera/MxCamera.h"
#include "../Window/MxWindow.h"

namespace Mix {
	Scene::Scene(std::string _name, const size_t _buildIdx) : mName(std::move(_name)),
		mBuildIndex(_buildIdx),
		mIsLoaded(false) {
// todo: manage Scene by SceneMgr
		mIsLoaded = true;

	}

	void Scene::awake() {
		auto model = MixEngine::Instance().getModule<Resource::ResourceLoader>()->load<Resource::Model>("TestResources/Model/DamagedHelmet/glTF/DamagedHelmet.gltf");
		auto object = model->generateGameObject({ "Model" });

		auto camera = new GameObject({ "Camera" });
		camera->addComponent<Camera>(Math::Vector3f(0.0f, 0.0f, 3.0f), object->transform().getPosition(), MixEngine::Instance().getWindow().extent());

		addGameObject(object);
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
