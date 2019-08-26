#pragma once
#ifndef MX_MESH_RENDERER_H_
#define MX_MESH_RENDERER_H_

#include "../MxComponent.h"

namespace Mix {
	class Material;

	namespace Vulkan {
		class VulkanAPI;
	}

	class Renderer :public Component {
		MX_DECLARE_RTTI;
		MX_DECLARE_CLASS_FACTORY;

		friend class Vulkan::VulkanAPI;
	public:
		std::shared_ptr<Material> getMaterial() const;

		const std::vector<std::shared_ptr<Material>>& getMaterials() const;

		void setMaterial(std::shared_ptr<Material> _material);

		void setMaterials(const std::vector<std::shared_ptr<Material>>& _materials);

		void setMaterials(std::vector<std::shared_ptr<Material>>&& _materials);

		bool isEnable() const { return mEnable; }

		void setEnable(const bool _enable) { mEnable = _enable; }

	private:
		std::vector<std::shared_ptr<Material>> mMaterials;
		bool mEnable = true;
	};

	class MeshRenderer final :public Renderer {
		MX_DECLARE_RTTI;
		MX_DECLARE_CLASS_FACTORY;
	};
}

#endif
