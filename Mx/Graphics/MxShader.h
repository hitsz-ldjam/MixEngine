#pragma once
#ifndef MX_SHADER_H_
#define MX_SHADER_H_

#include "../Math/MxMatrix4.h"
#include <optional>
#include <string>
#include <utility>
#include <any>
#include <unordered_set>
#include "../Utils/MxArrayProxy.h"

namespace Mix {
	class Camera;
	class Material;
	class Texture;
	struct RenderElement;

	namespace Vulkan {
		class ShaderBase;
	}

	enum class MaterialPropertyType {
		INT = 0x0000,
		FLOAT = 0x0001,
		MATRIX = 0x002,
		VECTOR = 0x0004,
		TEX_2D = 0x008,
	};

	struct MaterialPropertyInfo {
		std::string name;
		MaterialPropertyType type;
		std::any defaultValue;


		MaterialPropertyInfo(const std::string& _name, MaterialPropertyType _type, const std::any& _defaultValue)
			: name(_name),
			type(_type),
			defaultValue(_defaultValue) {
		}

		struct Hash {
			size_t operator()(const MaterialPropertyInfo& _key) const noexcept {
				return  std::hash<std::string>()(_key.name);
			}
		};

		bool operator==(const MaterialPropertyInfo& _other) const {
			return name == _other.name && type == _other.type;
		}

		bool operator!=(const MaterialPropertyInfo& _other) const { return !(*this == _other); }
	};

	using MaterialPropertySet = std::unordered_set<MaterialPropertyInfo, MaterialPropertyInfo::Hash>;

	class MaterialPropertyBlock {
	public:
		explicit MaterialPropertyBlock(const MaterialPropertySet& _set);

		std::optional<int>				getInt(const std::string& _name) const;
		std::optional<float>			getFloat(const std::string& _name) const;
		std::optional<Math::Matrix4>	getMatrix(const std::string& _name)	const;
		std::optional<Math::Vector4f>	getVector(const std::string& _name)	const;
		std::shared_ptr<Texture>		getTexture(const std::string& _name) const;

		void setInt(const std::string& _name, int _value);
		void setFloat(const std::string& _name, float _value);
		void setMatrix(const std::string& _name, const Math::Matrix4& _value);
		void setVector(const std::string& _name, const Math::Vector4f& _value);
		void setTexture(const std::string& _name, std::shared_ptr<Texture> _value);

		void clear() { mPropertyMap.clear(); }

		bool empty() const { return mPropertyMap.empty(); }

		bool hasProperty(const std::string& _name) const;
	private:
		struct Property {
			MaterialPropertyType type;
			std::any value;
		};

		using PropertyMap = std::unordered_map<std::string, Property>;
		PropertyMap mPropertyMap;
	};

	class Shader {
		friend class Vulkan::ShaderBase;
		friend class Graphics;
	public:
		std::optional<int>				getGlobalInt(const std::string& _name) const { return mGlobalProperties.getInt(_name); }
		std::optional<float>			getGlobalFloat(const std::string& _name) const { return mGlobalProperties.getFloat(_name); }
		std::optional<Math::Matrix4>	getGlobalMatrix(const std::string& _name) const { return mGlobalProperties.getMatrix(_name); }
		std::optional<Math::Vector4f>	getGlobalVector(const std::string& _name) const { return mGlobalProperties.getVector(_name); }
		std::shared_ptr<Texture>		getGlobalTexture(const std::string& _name) const { return mGlobalProperties.getTexture(_name); }

		void setGlobalInt(const std::string& _name, int _value);
		void setGlobalFloat(const std::string& _name, float _value);
		void setGlobalMatrix(const std::string& _name, const Math::Matrix4& _value);
		void setGlobalVector(const std::string& _name, const Math::Vector4f& _value);
		void setGlobalTexture(const std::string& _name, std::shared_ptr<Texture> _value);

		const std::string& name() const { return mName; }

		const auto& getMaterialPropertySet() const { return mPropertySet; }

		uint32_t _newMaterial();

		void _deleteMaterial(uint32_t _id);

		void _updated();

		void _render(ArrayProxy<const RenderElement> _renderElements, const Camera& _camera);
	private:
		Shader(std::shared_ptr<Vulkan::ShaderBase> _shader, uint32_t _id, std::string _name, const MaterialPropertySet& _shaerPropertySet, const MaterialPropertySet& _materialPropertySet)
			:mShader(std::move(_shader)),
			mShaderId(_id),
			mName(std::move(_name)),
			mGlobalProperties(_shaerPropertySet),
			mPropertySet(_materialPropertySet) {
		}

		Shader(std::shared_ptr<Vulkan::ShaderBase> _shader, uint32_t _id, std::string _name, const MaterialPropertySet& _shaerPropertySet, MaterialPropertySet&& _materialPropertySet)
			:mShader(std::move(_shader)),
			mShaderId(_id),
			mName(std::move(_name)),
			mGlobalProperties(_shaerPropertySet),
			mPropertySet(std::move(_materialPropertySet)) {
		}

		std::shared_ptr<Vulkan::ShaderBase> mShader;
		uint32_t mShaderId;
		const std::string mName;
		std::vector<std::string> mChangedList;
		MaterialPropertyBlock mGlobalProperties;
		MaterialPropertySet mPropertySet;
	};
}

#endif
