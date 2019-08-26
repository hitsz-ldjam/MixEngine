#include "MxScene.h"
#include "../GameObject/MxGameObject.h"
#include "../Resource/Model/MxModel.h"
#include "../../MixEngine.h"
#include "../Resource/MxResourceLoader.h"
#include "../BuildIn/Camera/MxCamera.h"
#include "../Window/MxWindow.h"
#include "../../Scripts/Test.h"
#include "../Resource/Texture/MxTexture.h"
#include "../Graphics/MxMaterial.h"
#include "../Component/Renderer/MxRenderer.h"
#include "../Graphics/MxGraphics.h"

namespace Mix {
	Scene::Scene(std::string _name, const size_t _buildIdx) : mName(std::move(_name)),
		mBuildIndex(_buildIdx),
		mIsLoaded(false) {
// todo: manage Scene by SceneMgr
		mIsLoaded = true;

	}

	void Scene::awake() {
		auto model0 = ResourceLoader::Get()->load<Model>("TestResources/Model/DamagedHelmet/glTF/DamagedHelmet.gltf");
		auto object0 = model0->generateGameObject({ "Model0" });

		auto param = GetAdditionalParam<Texture2D>();
		param.mipLevel = 0;
		param.samplerInfo.wrapModeU = TextureWrapMode::CLAMP_TO_EDGE;
		param.samplerInfo.wrapModeV = TextureWrapMode::CLAMP_TO_EDGE;
		param.samplerInfo.wrapModeW = TextureWrapMode::CLAMP_TO_EDGE;
		param.samplerInfo.minFilter = TextureFilterMode::LINEAR;
		param.samplerInfo.magFilter = TextureFilterMode::LINEAR;
		param.samplerInfo.mipFilter = TextureMipSampleMode::LINEAR;

		auto baseColorTex = ResourceLoader::Get()->load<Texture2D>("TestResources/Model/DamagedHelmet/glTF/Default_albedo.jpg", &param);
		auto normalTex = ResourceLoader::Get()->load<Texture2D>("TestResources/Model/DamagedHelmet/glTF/Default_normal.jpg", &param);

		auto material = std::make_shared<Material>(*Graphics::Get()->findShader("Standard"));
		material->setTexture("baseColorTex", baseColorTex);
		material->setTexture("normalTex", normalTex);

		auto child = object0->getAllChildren()[0];
		auto renderer = child->addComponent<Renderer>();
		renderer->setMaterial(material);

		/*auto model1 = ResourceLoader::Get()->load<Model>("TestResources/Model/BoomBoxWithAxes/glTF/BoomBoxWithAxes.gltf");
		auto object1 = model1->generateGameObject({ "Model1" });

		object1->transform().translate(3.0f, 0.0f, 0.0f, Space::WORLD);
		object1->transform().setLocalScale(Math::Vector3f(50.0f, 50.0f, 50.0f));*/

		auto camera = new GameObject({ "Camera" });
		camera->transform().setPosition(Math::Vector3f(0.0f, 0.0f, 3.0f));
		camera->addComponent<Camera>(Window::Get()->extent());
		camera->addComponent<Scripts::TestScript>();

		addGameObject(object0);
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
