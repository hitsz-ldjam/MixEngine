#pragma once
#ifndef MX_VK_RENDERER_BASE_H_
#define MX_VK_RENDERER_BASE_H_

#include "../../Utils/MxGeneralBase.hpp"
#include "../../Graphics/MxMaterial.h"
#include "../../Utils/MxArrayProxy.h"
#include "../../Graphics/MxRenderQueue.h"

namespace Mix {
    class Camera;
    class Mesh;

    namespace Vulkan {
        class CommandBufferHandle;

        class ShaderBase : public GeneralBase::NoCopyBase {
        public:
            ShaderBase(VulkanAPI* _vulkan) :mVulkan(_vulkan) {}

            virtual ~ShaderBase() = default;

            virtual void beginRender(const Camera& _camera) = 0;

            virtual void render(ArrayProxy<RenderQueueElement> _elements) = 0;

            virtual void endRender() = 0;

            virtual void update(const Shader& _shader) = 0;

            const MaterialPropertySet& getMaterialPropertySet() const { return mMaterialPropertySet; }

            const MaterialPropertySet& getShaderPropertySet() const { return mShaderPropertySet; }

            virtual uint32_t newMaterial() = 0;

            virtual void deleteMaterial(uint32_t _id) = 0;

        protected:
            VulkanAPI* mVulkan;
            MaterialPropertySet mMaterialPropertySet;
            MaterialPropertySet mShaderPropertySet;

            static void DrawMesh(CommandBufferHandle& _cmd, const Mesh& _mesh, uint32_t _submesh);
        };
    }
}

#endif
