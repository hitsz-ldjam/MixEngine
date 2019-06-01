#pragma once
#include "../Core/MxVkCore.h"
#include "MxVkBuffer.h"
#include "../Descriptor/MxVkDescriptor.h"
#include "../../Utils/MxOffsetSize.hpp"

#include <cmath>

namespace Mix {
    namespace Graphics {
        class DynamicUniformBuffer {
        public:
            explicit DynamicUniformBuffer(const std::shared_ptr<Core>& _core,
                                          const std::shared_ptr<DeviceAllocator>& _allocator,
                                          const uint32_t _sizePerObj,
                                          const uint32_t _count) :mCurrCount(0) {
                const auto align = _core->getPhysicalDeviceProperties().limits.minUniformBufferOffsetAlignment;
                const auto maxSize = _core->getPhysicalDeviceProperties().limits.maxUniformBufferRange;

                const auto actualAlign = Math::Align(_sizePerObj, static_cast<uint32_t>(align));

                const auto maxCount = maxSize / actualAlign;
                const auto actualCount = std::min(maxCount, _count);

                mUniformSize = actualAlign;
                mMaxUniformCount = actualCount;

                mBuffer = Buffer::CreateBufferUnique(_core,
                                                     _allocator,
                                                     vk::BufferUsageFlagBits::eUniformBuffer,
                                                     vk::MemoryPropertyFlagBits::eHostVisible |
                                                     vk::MemoryPropertyFlagBits::eHostCoherent,
                                                     actualCount * actualAlign,
                                                     vk::SharingMode::eExclusive);
                mBuffer->setupDescriptor();
            }

            void reset() {
                mCurrCount = 0;
            }

            void next() {
                ++mCurrCount;
            }

            WriteDescriptorSet getWriteDescriptorSet(const vk::DescriptorSet& _set,
                                                     const uint32_t& _binding,
                                                     std::optional<Utils::OffsetSize<uint32_t>> _offsetSize = std::nullopt) const {
                vk::DescriptorBufferInfo bufferInfo = mBuffer->descriptor;
                if (_offsetSize) {
                    bufferInfo.offset = _offsetSize.value().offset;
                    bufferInfo.range = _offsetSize.value().size;
                }

                vk::WriteDescriptorSet write;
                write.dstSet = _set; //descriptor which will be write in
                write.dstBinding = _binding; //destination binding
                write.dstArrayElement = 0;
                write.descriptorType = vk::DescriptorType::eUniformBufferDynamic; //the type of the descriptor that will be wirte in
                write.descriptorCount = 1; //descriptor count
                write.pBufferInfo = &bufferInfo; //descriptorBufferInfo
                write.pImageInfo = nullptr;
                write.pTexelBufferView = nullptr;

                return WriteDescriptorSet(write, bufferInfo);
            }

            uint32_t getAlign() const {
                return mUniformSize * mCurrCount;
            }

            void pushBack(const void* _data, const vk::DeviceSize& _size) const {
                mBuffer->copyTo(_data, getAlign(), _size);
            }

            bool full() const {
                return mCurrCount >= mMaxUniformCount;
            }

        private:
            std::unique_ptr<Buffer> mBuffer;

            uint32_t mUniformSize;
            uint32_t mMaxUniformCount;

            uint32_t mCurrCount;
        };

    };
}

