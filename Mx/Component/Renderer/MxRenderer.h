#pragma once
#ifndef MX_MESH_RENDERER_H_
#define MX_MESH_RENDERER_H_

#include "../MxComponent.h"
#include <unordered_map>

namespace Mix {
	class Material;

	class Renderer :public Component {
		MX_DECLARE_RTTI;
		MX_DECLARE_CLASS_FACTORY;
	public:
		explicit Renderer(std::shared_ptr<Material> _material) :mMaterial(std::move(_material)) {}

		std::shared_ptr<Material> getMaterial() const { return mMaterial; }

		void setMaterial(std::shared_ptr<Material> _material) { mMaterial = std::move(_material); }

		bool isEnable() const { return mEnable; }

		void setEnable(const bool _enable) { mEnable = _enable; }

	private:
		std::shared_ptr<Material> mMaterial;
		bool mEnable = true;
	};
}

#endif
