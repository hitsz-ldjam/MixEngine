#pragma once
#ifndef _MX_VK_BUFFER_H_
#define _MX_VK_BUFFER_H_


#include "MxVkCore.h"
#include "MxVkCommand.h"


namespace Mix {
    namespace Graphics {
        class Buffer :GraphicsComponent {
            MX_DECLARE_RTTI;
            MX_DECLARE_CLASS_FACTORY;
        public:
            vk::Buffer buffer;
            vk::DeviceMemory memory;
            vk::DescriptorBufferInfo descriptor;
            vk::DeviceSize size;
            vk::DeviceSize alignment;
            void* mapped;

            vk::BufferUsageFlags usages;
            vk::MemoryPropertyFlags memoryProperty;

            virtual ~Buffer() { destory(); }

            void map(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void unmap();
            void bind(const vk::DeviceSize offset = 0);
            void setupDescriptor(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void copyTo(const void * data, const vk::DeviceSize size);
            void flush(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void invalidate(const vk::DeviceSize size = VK_WHOLE_SIZE, const vk::DeviceSize offset = 0);
            void destory();

            static Buffer* createBuffer(const Core* core, const vk::BufferUsageFlags usage,
                                        const vk::MemoryPropertyFlags memoryProperty,
                                        const vk::DeviceSize size,
                                        const vk::SharingMode sharingMode = vk::SharingMode::eExclusive, 
                                        const void * data = nullptr);

            static void copyToDeviceBuffer(const Core* core,
                                           CommandMgr* commandPool, 
                                           const Buffer* dstBuffer, 
                                           const void* data);
        };
    }

}
#endif // !_MX_VK_BUFFER_H_
