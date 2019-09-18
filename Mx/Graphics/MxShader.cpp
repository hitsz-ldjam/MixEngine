#include "MxShader.h"
#include "../Vulkan/Shader/MxVkShaderBase.h"

namespace Mix {
    MaterialPropertyBlock::MaterialPropertyBlock(const MaterialPropertySet& _set) {
        for (auto& property : _set) {
            switch (property.type) {
            case MaterialPropertyType::INT: setInt(property.name, std::any_cast<int>(property.defaultValue)); break;
            case MaterialPropertyType::FLOAT: setFloat(property.name, std::any_cast<float>(property.defaultValue)); break;
            case MaterialPropertyType::MATRIX: setMatrix(property.name, std::any_cast<Matrix4>(property.defaultValue)); break;
            case MaterialPropertyType::VECTOR: setVector(property.name, std::any_cast<Vector4f>(property.defaultValue)); break;
            case MaterialPropertyType::TEX_2D: setTexture(property.name, std::any_cast<std::shared_ptr<Texture>>(property.defaultValue)); break;
            }
        }
    }

    void Shader::setGlobalInt(const std::string& _name, int _value) {
        if (mGlobalProperties.hasProperty(_name)) {
            mGlobalProperties.setInt(_name, _value);
            mChangedList.push_back(_name);
        }
    }

    void Shader::setGlobalFloat(const std::string& _name, float _value) {
        if (mGlobalProperties.hasProperty(_name)) {
            mGlobalProperties.setFloat(_name, _value);
            mChangedList.push_back(_name);
        }
    }

    void Shader::setGlobalMatrix(const std::string& _name, const Matrix4& _value) {
        if (mGlobalProperties.hasProperty(_name)) {
            mGlobalProperties.setMatrix(_name, _value);
            mChangedList.push_back(_name);
        }
    }

    void Shader::setGlobalVector(const std::string& _name, const Vector4f& _value) {
        if (mGlobalProperties.hasProperty(_name)) {
            mGlobalProperties.setVector(_name, _value);
            mChangedList.push_back(_name);
        }
    }

    void Shader::setGlobalTexture(const std::string& _name, std::shared_ptr<Texture> _value) {
        if (mGlobalProperties.hasProperty(_name)) {
            mGlobalProperties.setTexture(_name, std::move(_value));
            mChangedList.push_back(_name);
        }
    }

    void Shader::update() {
        mShader->update(*this);
        mChangedList.clear();
    }

    uint32_t Shader::_newMaterial() {
        return mShader->newMaterial();
    }

    void Shader::_deleteMaterial(uint32_t _id) {
        mShader->deleteMaterial(_id);
    }

    void Shader::beginRender(const Camera& _camera) {
        mShader->beginRender(_camera);
    }

    void Shader::render(RenderElement& _element) {
        mShader->render(_element);
    }

    void Shader::endRender() {
        mShader->endRender();
    }
}
