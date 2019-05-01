#pragma once
#ifndef _MX_VK_IMAGE_H_
#define _MX_VK_IMAGE_H_


#include "MxVkCore.h"


namespace Mix {
    namespace Graphics {
        struct Image {
            vk::Image image;
            vk::DeviceMemory memory;
            vk::Format format;
            vk::Extent3D extent;
        };


        namespace Tools {
            vk::Image createImage2D(const Core& core,
                                    const vk::Extent2D& extent,
                                    const vk::Format format,
                                    const vk::ImageUsageFlags& usage,
                                    const uint32_t mipLevels = 1, const uint32_t arrayLayers = 1,
                                    const vk::SampleCountFlagBits sampleCount = vk::SampleCountFlagBits::e1,
                                    const vk::ImageLayout initialLayout = vk::ImageLayout::eUndefined,
                                    const vk::ImageTiling tiling = vk::ImageTiling::eOptimal,
                                    const vk::SharingMode sharingMode = vk::SharingMode::eExclusive);

            vk::ImageView createImageView2D(const vk::Device& device,
                                            const vk::Image& image,
                                            const vk::Format format,
                                            const vk::ImageAspectFlags& aspectFlags,
                                            const uint32_t baseMipLevel = 0,
                                            const uint32_t levelCount = 1,
                                            const uint32_t baseLayer = 0,
                                            const uint32_t layerCount = 1);

            vk::DeviceMemory allocateImageMemory(const Core& core,
                                                 const vk::Image& image,
                                                 const vk::MemoryPropertyFlags& properties);

            void transferImageLayout(const vk::CommandBuffer& cmdbuffer,
                                     vk::Image image,
                                     vk::ImageLayout oldImageLayout,
                                     vk::ImageLayout newImageLayout,
                                     const vk::ImageSubresourceRange& subresourceRange,
                                     vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
                                     vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eAllCommands);

            Image createDepthStencil(const Core& core,

                                      const vk::Extent2D& extent,

                                      const vk::SampleCountFlagBits sampleCount);
        }

        /*class ImageMgr :public GraphicsComponent {
        // todo
        };*/
    }
}

#endif // !_MX_VK_IMAGE_H_
