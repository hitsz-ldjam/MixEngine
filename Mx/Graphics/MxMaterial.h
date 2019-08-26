#pragma once
#ifndef MX_MATERIAL_H_
#define MX_MATERIAL_H_

#include "MxShader.h"
#include "../Math/MxMatrix4.h"
#include <string>
#include <optional>

namespace Mix {
	class Texture;

	namespace Vulkan {
		class VulkanAPI;
		class ShaderBase;
		class DescriptorSet;
	}

	class Material {
		friend class Vulkan::VulkanAPI;
		friend class Vulkan::ShaderBase;
	public:
		explicit Material(Shader& _shader);

		~Material();

		std::optional<int>				getInt(const std::string& _name) const { return mMaterialProperties.getInt(_name); }
		std::optional<float>			getFloat(const std::string& _name) const { return mMaterialProperties.getFloat(_name); }
		std::optional<Math::Matrix4>	getMatrix(const std::string& _name)	const { return mMaterialProperties.getMatrix(_name); }
		std::optional<Math::Vector4f>	getVector(const std::string& _name)	const { return mMaterialProperties.getVector(_name); }
		std::shared_ptr<Texture>		getTexture(const std::string& _name) const { return mMaterialProperties.getTexture(_name); }

		void setInt(const std::string& _name, int _value);
		void setFloat(const std::string& _name, float _value);
		void setMatrix(const std::string& _name, const Math::Matrix4& _value);
		void setVector(const std::string& _name, const Math::Vector4f& _value);
		void setTexture(const std::string& _name, std::shared_ptr<Texture> _value);

		void _updated();

		uint32_t _getMaterialId() const { return mMaterialId; }

		const std::unordered_set<std::string>& _getChangedList() const { return mChangedList; }
	private:

		uint32_t mMaterialId;
		Shader& mShader;
		std::unordered_set<std::string> mChangedList;
		MaterialPropertyBlock mMaterialProperties;
	};
}

#endif
