#include "MxMaterial.h"
#include "../Vulkan/Descriptor/MxVkDescriptorSet.h"
#include "../Graphics/Texture/MxTexture.h"

namespace Mix {
    std::optional<int> MaterialPropertyBlock::getInt(const std::string& _name) const {
        auto it = mPropertyMap.find(_name);
        if (it != mPropertyMap.end() && it->second.type == MaterialPropertyType::INT)
            return std::any_cast<int>(it->second.value);
        return std::nullopt;
    }

    std::optional<float> MaterialPropertyBlock::getFloat(const std::string& _name) const {
        auto it = mPropertyMap.find(_name);
        if (it != mPropertyMap.end() && it->second.type == MaterialPropertyType::FLOAT)
            return std::any_cast<float>(it->second.value);
        return std::nullopt;
    }

    std::optional<Matrix4> MaterialPropertyBlock::getMatrix(const std::string& _name) const {
        auto it = mPropertyMap.find(_name);
        if (it != mPropertyMap.end() && it->second.type == MaterialPropertyType::MATRIX)
            return std::any_cast<Matrix4>(it->second.value);
        return std::nullopt;
    }

    std::optional<Vector4f> MaterialPropertyBlock::getVector(const std::string& _name) const {
        auto it = mPropertyMap.find(_name);
        if (it != mPropertyMap.end() && it->second.type == MaterialPropertyType::VECTOR)
            return std::any_cast<Vector4f>(it->second.value);
        return std::nullopt;
    }

    std::shared_ptr<Texture> MaterialPropertyBlock::getTexture(const std::string& _name) const {
        auto it = mPropertyMap.find(_name);
        if (it != mPropertyMap.end() && it->second.type == MaterialPropertyType::TEX_2D)
            return std::any_cast<std::shared_ptr<Texture>>(it->second.value);
        return nullptr;
    }

    void MaterialPropertyBlock::setInt(const std::string& _name, int _value) {
        mPropertyMap[_name] = Property{ MaterialPropertyType::INT, _value };
    }

    void MaterialPropertyBlock::setFloat(const std::string& _name, float _value) {
        mPropertyMap[_name] = Property{ MaterialPropertyType::FLOAT, _value };
    }

    void MaterialPropertyBlock::setMatrix(const std::string& _name, const Matrix4& _value) {
        mPropertyMap[_name] = Property{ MaterialPropertyType::MATRIX, _value };
    }

    void MaterialPropertyBlock::setVector(const std::string& _name, const Vector4f& _value) {
        mPropertyMap[_name] = Property{ MaterialPropertyType::VECTOR, _value };
    }

    void MaterialPropertyBlock::setTexture(const std::string& _name, std::shared_ptr<Texture> _value) {
        mPropertyMap[_name] = Property{ MaterialPropertyType::TEX_2D, _value };
    }

    bool MaterialPropertyBlock::hasProperty(const std::string& _name) const {
        return mPropertyMap.find(_name) != mPropertyMap.end();
    }

    Material::Material(const std::shared_ptr<Shader>& _shader)
        :mMaterialId(_shader->_newMaterial()),
        mShader(_shader),
        mMaterialProperties(_shader->getMaterialPropertySet()),
        mRenderType(RenderType::Opaque) {
        for (auto& property : mShader->getMaterialPropertySet()) {
            mChangedList.insert(property.name);
        }
    }

    Material::~Material() {
        mShader->_deleteMaterial(mMaterialId);
    }

    void Material::setInt(const std::string& _name, int _value) {
        if (mMaterialProperties.hasProperty(_name)) {
            mMaterialProperties.setInt(_name, _value);
            mChangedList.insert(_name);
        }
    }

    void Material::setFloat(const std::string& _name, float _value) {
        if (mMaterialProperties.hasProperty(_name)) {
            mMaterialProperties.setFloat(_name, _value);
            mChangedList.insert(_name);
        }
    }

    void Material::setMatrix(const std::string& _name, const Matrix4& _value) {
        if (mMaterialProperties.hasProperty(_name)) {
            mMaterialProperties.setMatrix(_name, _value);
            mChangedList.insert(_name);
        }
    }

    void Material::setVector(const std::string& _name, const Vector4f& _value) {
        if (mMaterialProperties.hasProperty(_name)) {
            mMaterialProperties.setVector(_name, _value);
            mChangedList.insert(_name);
        }
    }

    void Material::setTexture(const std::string& _name, std::shared_ptr<Texture> _value) {
        if (mMaterialProperties.hasProperty(_name)) {
            mMaterialProperties.setTexture(_name, std::move(_value));
            mChangedList.insert(_name);
        }
    }

    void Material::_updated() {
        mChangedList.clear();
    }
}
