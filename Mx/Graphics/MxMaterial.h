#pragma once
#ifndef MX_MATERIAL_H_
#define MX_MATERIAL_H_

#include "MxShader.h"
#include "../Math/MxMatrix4.h"
#include <string>
#include <optional>
#include "../Definitions/MxCommonType.h"

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
        explicit Material(const std::shared_ptr<Shader>& _shader);

        ~Material();

        std::optional<int>				getInt(const std::string& _name) const { return mMaterialProperties.getInt(_name); }
        std::optional<float>			getFloat(const std::string& _name) const { return mMaterialProperties.getFloat(_name); }
        std::optional<Matrix4>	        getMatrix(const std::string& _name)	const { return mMaterialProperties.getMatrix(_name); }
        std::optional<Vector4f>	        getVector(const std::string& _name)	const { return mMaterialProperties.getVector(_name); }
        std::shared_ptr<Texture>		getTexture(const std::string& _name) const { return mMaterialProperties.getTexture(_name); }

        void setInt(const std::string& _name, int _value);
        void setFloat(const std::string& _name, float _value);
        void setMatrix(const std::string& _name, const Matrix4& _value);
        void setVector(const std::string& _name, const Vector4f& _value);
        void setTexture(const std::string& _name, std::shared_ptr<Texture> _value);

        void setRenderType(RenderType _mode) { mRenderType = _mode; }

        RenderType getRenderType() const { return mRenderType; };

        void _updated();

        uint32_t _getMaterialId() const { return mMaterialId; }

        const std::unordered_set<std::string>& _getChangedList() const { return mChangedList; }

        std::shared_ptr<Shader> getShader() const { return mShader; }

        void setGPUInstancing(bool _enable) { mInstancing = _enable; }

        bool getGPUInstancing() const { return mInstancing; }

    private:

        uint32_t mMaterialId;
        std::shared_ptr<Shader> mShader;
        std::unordered_set<std::string> mChangedList;
        MaterialPropertyBlock mMaterialProperties;

        RenderType mRenderType;
        bool mInstancing = false;
    };
}

#endif
