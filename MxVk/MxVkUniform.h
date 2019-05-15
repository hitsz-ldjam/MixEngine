#pragma once
#ifndef MX_VK_RENDER_H_
#define MX_VK_RENDER_H_

#include <map>
#include <memory>
#include "MxVkCore.h"
#include "MxVkBuffer.h"
#include "MxVkDescriptor.h"
#include "../Utils/MxUtils.h"


namespace Mix {
    namespace Graphics {
        struct AbstractUniform {
        protected:
            AbstractUniform() = default;
        };

        struct AbstractLightUniform :public AbstractUniform {
            glm::vec4 color;
        protected:
            AbstractLightUniform() :color() {}
        };

        struct AbstractCameraUniform :public AbstractUniform {
            glm::vec4 position;
            glm::vec4 forward;
            glm::mat4 viewMat;
            glm::mat4 projMat;
        protected:
            AbstractCameraUniform() : position(), forward(), viewMat(), projMat() {
            }
        };

        struct MeshUniform :public AbstractUniform {
            glm::mat4 modelMat;
            glm::mat4 normMat;
        };

        struct DirectionLightUniform :public AbstractLightUniform {
            glm::vec4 direction;
        };

        struct PointLightUniform :public AbstractLightUniform {
            float constant;
            glm::vec3 position;
            float linear;
            float quadratic;
        };

        struct SpotLightUniform :public AbstractLightUniform {
            glm::vec4 position;
            glm::vec4 direction;
            float innerCutoff;
            float outerCutoff;
            float constant;
            float linear;
            float quadratic;

        };

        struct CameraUniform :public AbstractCameraUniform {
            CameraUniform() = default;
        };


        /*class UniformMgr :GraphicsComponent {
        public:
            void init(std::shared_ptr<Core> core, std::shared_ptr<DeviceAllocator> allocator);

            Id createRenderableObj();
            std::weak_ptr<Buffer> getRenderableObj(const Id& id, size_t currFrame) {
                if (mRenderableObjs.count(id) == 0)
                    return std::weak_ptr<Buffer>();

                return mRenderableObjs[id];
            }

            Id createCamera();
            std::weak_ptr<Buffer> getCamera(const Id& id, size_t currFrame) {
                if (mCameras.count(id) == 0)
                    return std::weak_ptr<Buffer>();

                return mCameras[id];
            }

        private:
            Utils::NextIdGenerator mRenderableObjId;
            std::unordered_map<Id, std::shared_ptr<Buffer>> mRenderableObjs;
            Utils::NextIdGenerator mCameraId;
            std::unordered_map<Id, std::shared_ptr<Buffer>> mCameras;

            std::shared_ptr<DeviceAllocator> mAllocator;

            template<typename T>
            std::unique_ptr<Buffer> createUniformBuffer();
        };

        template<typename T>
        inline std::unique_ptr<Buffer> UniformMgr::createUniformBuffer() {
            return std::unique_ptr<Buffer>(Buffer::createBuffer(mCore,
                                           mAllocator,
                                           vk::BufferUsageFlagBits::eUniformBuffer,
                                           vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent,
                                           sizeof(T)));
        }*/
    }
}

#endif // !MX_VK_RENDER_H_
