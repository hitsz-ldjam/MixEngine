#include "MxVkBuffer.h"

namespace Mix {
    namespace Graphics {
        MX_IMPLEMENT_RTTI_NoCreateFunc(Buffer, GraphicsComponent);
        MX_IMPLEMENT_DEFAULT_CLASS_FACTORY(Buffer);

        Buffer * Buffer::createBuffer(const Core* core, const vk::BufferUsageFlags usage,
                                      const vk::MemoryPropertyFlags memoryProperty,
                                      const vk::DeviceSize size,
                                      const vk::SharingMode sharingMode, const void * data) {
            Buffer* buffer = new Buffer;
            buffer->init(core);

            vk::BufferCreateInfo createInfo;
            createInfo.size = size;
            createInfo.usage = usage;
            createInfo.sharingMode = sharingMode;

            buffer->buffer = core->device().createBuffer(createInfo);

            vk::MemoryRequirements memRequirements = core->device().getBufferMemoryRequirements(buffer->buffer);

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = memRequirements.size;
            allocInfo.memoryTypeIndex = core->getMemoryTypeIndex(memRequirements.memoryTypeBits, memoryProperty);
            buffer->memory = core->device().allocateMemory(allocInfo);

            buffer->alignment = memRequirements.alignment;
            buffer->size = size;
            buffer->usages = usage;
            buffer->memoryProperty = memoryProperty;

            if (data && memoryProperty & vk::MemoryPropertyFlagBits::eHostVisible) {
                buffer->map();
                buffer->copyTo(data, size);

                //if not HOST_COHERENT then need to be flushed
                if (!(memoryProperty & vk::MemoryPropertyFlagBits::eHostCoherent))
                    buffer->flush();
                buffer->unmap();
            }

            buffer->setupDescriptor(buffer->size, 0);
            buffer->bind();
            return buffer;
        }

        void Buffer::copyToDeviceBuffer(const Core * core, CommandMgr* commandPool, const Buffer* dstBuffer, const void * data) {
            if (!data)
                throw std::runtime_error("Error : Pointer [ data ] is NULL");

            Buffer* tempBuffer = createBuffer(core,
                                              vk::BufferUsageFlagBits::eTransferSrc,
                                              vk::MemoryPropertyFlagBits::eHostVisible |
                                              vk::MemoryPropertyFlagBits::eHostCoherent,
                                              dstBuffer->size,
                                              vk::SharingMode::eExclusive,
                                              data);

            vk::BufferCopy copyRegion = {};
            copyRegion.size = dstBuffer->size;
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;

            vk::CommandBuffer commandBuffer = commandPool->beginTempCommandBuffer();
            commandBuffer.copyBuffer(tempBuffer->buffer, dstBuffer->buffer, copyRegion);
            commandPool->endTempCommandBuffer(commandBuffer);
            delete tempBuffer;
        }

        void Buffer::map(const vk::DeviceSize size, const vk::DeviceSize offset) {
            mapped = mCore->device().mapMemory(memory, offset, size);
        }

        void Buffer::unmap() {
            if (mapped) {
                mCore->device().unmapMemory(memory);
                mapped = nullptr;
            }
        }

        void Buffer::bind(const vk::DeviceSize offset) {
            mCore->device().bindBufferMemory(buffer, memory, offset);
        }

        void Buffer::setupDescriptor(const vk::DeviceSize size, const vk::DeviceSize offset) {
            descriptor.offset = offset;
            descriptor.buffer = buffer;
            descriptor.range = size;
        }

        void Buffer::copyTo(const void * data, const vk::DeviceSize size) {
            assert(data);
            memcpy(mapped, data, static_cast<size_t>(size));
        }

        void Buffer::flush(const vk::DeviceSize size, const vk::DeviceSize offset) {
            vk::MappedMemoryRange mappedRange;
            mappedRange.memory = memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            mCore->device().flushMappedMemoryRanges(mappedRange);
        }

        void Buffer::invalidate(const vk::DeviceSize size, const vk::DeviceSize offset) {
            vk::MappedMemoryRange mappedRange = {};
            mappedRange.memory = memory;
            mappedRange.offset = offset;
            mappedRange.size = size;
            mCore->device().invalidateMappedMemoryRanges(mappedRange);
        }

        void Buffer::destory() {
            if (buffer) {
                mCore->device().destroyBuffer(buffer);
                buffer = VK_NULL_HANDLE;
            }
            if (memory) {
                mCore->device().freeMemory(memory);
                memory = VK_NULL_HANDLE;
            }
        }
    }
}