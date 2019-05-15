#pragma once
#ifndef _MX_RENDER_H_
#define _MX_RENDER_H_

#include <utility>
#include "MxMesh.h"
#include "../MxVk/MxVkUniform.h"

namespace Mix {
    class MeshRenderer :public Component {
    public:
        MeshRenderer(std::vector<std::shared_ptr<Graphics::Buffer>> _buffers,
                     std::vector<vk::DescriptorSet> _descriptorSets) :
            mBuffers(std::move(_buffers)), mDescriptor(std::move(_descriptorSets)) {
        }

        const Graphics::MeshUniform& uniform() {
            auto transform = mGameObj->getComponent<Transform>();
            mUniform.modelMat = glm::scale(glm::mat4(1.0f), transform->scale());
            mUniform.modelMat = glm::toMat4(transform->rotation())  * mUniform.modelMat;
            mUniform.modelMat = glm::translate(mUniform.modelMat, transform->position());
            mUniform.normMat = glm::transpose(glm::inverse(mUniform.modelMat));
            return mUniform;
        }

        std::shared_ptr<Graphics::Buffer> uniformBuffers(const size_t _currentFrame) {
            return mBuffers[_currentFrame];
        }

        const vk::DescriptorSet& descriptorSet(const size_t _currentFrame) {
            return mDescriptor[_currentFrame];
        }

    private:
        Graphics::MeshUniform mUniform;
        std::vector<std::shared_ptr<Graphics::Buffer>> mBuffers;
        std::vector<vk::DescriptorSet> mDescriptor;
    };
}

#endif // !_MX_RENDER_H_
