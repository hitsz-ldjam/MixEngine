#pragma once
#ifndef _MX_RENDER_H_
#define _MX_RENDER_H_

#include "MxMesh.h"
#include "../MxVk/MxVkUniform.h"

namespace Mix {
    struct RenderInfo {
        Mix::MeshFilter::MeshRef meshRef;
        // material
    };

    class MeshRenderer :public Component {
    public:
        MeshRenderer(const std::vector<std::shared_ptr<Graphics::Buffer>>& buffers, const std::vector<vk::DescriptorSet>& descriptorSets) :
            mBuffers(buffers), mDescriptor(descriptorSets) {
        }

        RenderInfo getRenderInfo() const {
            RenderInfo renderInfo;
            renderInfo.meshRef = mGameObj->getComponent<MeshFilter>()->getMeshRef();

            return renderInfo;
        }

        const Graphics::MeshUniform& uniform() {
            auto transform = mGameObj->getComponent<Transform>();
            mUniform.modelMat = glm::scale(glm::mat4(1.0f), transform->scale());
            mUniform.modelMat = glm::toMat4(transform->rotation())  * mUniform.modelMat;
            mUniform.modelMat = glm::translate(mUniform.modelMat, transform->position());

            mUniform.normMat = glm::transpose(glm::inverse(mUniform.modelMat));
            return mUniform;
        }

        std::shared_ptr<Graphics::Buffer> uniformBuffers(size_t currFrame) {
            return mBuffers[currFrame];
        }

        const vk::DescriptorSet& descriptorSet(size_t currFrame) {
            return mDescriptor[currFrame];
        }

    private:
        Graphics::MeshUniform mUniform;
        std::vector<std::shared_ptr<Graphics::Buffer>> mBuffers;
        std::vector<vk::DescriptorSet> mDescriptor;
    };
}

#endif // !_MX_RENDER_H_
