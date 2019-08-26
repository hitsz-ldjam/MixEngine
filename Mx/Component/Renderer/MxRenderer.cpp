#include "MxRenderer.h"

namespace Mix {
	MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(Renderer, Component);
	MX_IMPLEMENT_RTTI_NO_CREATE_FUNC(MeshRenderer, Renderer);

	std::shared_ptr<Material> Renderer::getMaterial() const {
		if (mMaterials.empty())
			return nullptr;
		return mMaterials[0];
	}

	const std::vector<std::shared_ptr<Material>>& Renderer::getMaterials() const {
		return mMaterials;
	}

	void Renderer::setMaterial(std::shared_ptr<Material> _material) {
		if (mMaterials.empty())
			mMaterials.push_back(std::move(_material));
		else
			mMaterials[0] = std::move(_material);
	}

	void Renderer::setMaterials(const std::vector<std::shared_ptr<Material>>& _materials) {
		mMaterials = _materials;
	}

	void Renderer::setMaterials(std::vector<std::shared_ptr<Material>>&& _materials) {
		mMaterials = std::move(_materials);
	}
}
