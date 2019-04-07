#pragma once
#ifndef _MX_VK_IMAGE_H_
#define _MX_VK_IMAGE_H_

#include "../Mx/MxComponent.h"
#include "../Utils/MxUtils.h"
#include "MxVkCore.h"
#include "MxVkAllocator.h"
#include <algorithm>
#include <unordered_map>


namespace Mix {
    namespace Graphics {
        struct Image {
            vk::Image image;
            vk::DeviceMemory memory;
            vk::Format format;
            vk::Extent3D extent;
        };


        namespace Tools {
            vk::Image createImage2D(Core& core,
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

            vk::DeviceMemory allocateImageMemory(Core& core,
                                                 const vk::Image& image,
                                                 const vk::MemoryPropertyFlags& properties);

            void transferImageLayout(const vk::CommandBuffer& cmdbuffer,
                                     vk::Image image,
                                     vk::ImageLayout oldImageLayout,
                                     vk::ImageLayout newImageLayout,
                                     const vk::ImageSubresourceRange& subresourceRange,
                                     vk::PipelineStageFlags srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
                                     vk::PipelineStageFlags dstStageMask = vk::PipelineStageFlagBits::eAllCommands);

            Image createDepthStencil(Core& core,
                                     const vk::Extent2D& extent,
                                     const vk::SampleCountFlagBits sampleCount);
        }

        struct ImageInfo {
            vk::ImageType type;
            vk::Image image;
            vk::Format format;
            vk::Extent3D extent;
            vk::DeviceSize size;
            uint32_t mipLevels;
            uint32_t arrayLevels;
        };

        class ImageMgr :public GraphicsComponent {
        private:
            struct TempData {
                std::string name;
                ImageInfo imageInfo;
                const char* ptr;

                TempData() :name(), imageInfo(), ptr(nullptr) {}

                TempData(const std::string& n, const ImageInfo& i, const char* p) :
                    name(n), imageInfo(i), ptr(p) {
                }
            };
        public:
            virtual ~ImageMgr() {
                destroy();
            }
            void init(std::shared_ptr<Core>& core, std::shared_ptr<DeviceAllocator>& allocator);

            void beginLoad(const vk::CommandBuffer& cmd);
            void loadTexture(const std::string& name, const gli::texture& texture);
            void endLoad();

            const ImageInfo& getImage(const std::string& name) {
                if(mImageInfos.count(name)==0)
                    throw std::runtime_error("Error : No image named [ " + name + " ]");

                return mImageInfos[name].first;
            }

            void destroy();
        private:
            std::vector<TempData> mDatas;
            bool mBegin = false;
            vk::DeviceSize mCurrSize = 0;
            vk::CommandBuffer mCmd;
            bool mCmdBegin = false;

            vk::DeviceSize mBufferSize = 8 * 1024 * 1024;

            vk::Buffer mStagingBuffer;
            MemoryBlock mMemBlock;
            vk::MemoryRequirements mMemReq;
            Fence mFence;

            std::unordered_map<std::string, std::pair<ImageInfo, MemoryBlock>> mImageInfos;

            std::shared_ptr<DeviceAllocator> mpAllocator;

            void flush();

            vk::DeviceSize leastMultiple(vk::DeviceSize a, vk::DeviceSize b) {
                vk::DeviceSize m = a, n = b, c;
                while (b != 0) {
                    c = a % b;
                    a = b;
                    b = c;
                }
                return m * n / a;
            }

            vk::Format gliToVulkanFormat(gli::format format);

            void loadImage2D(const std::string& name, const gli::texture& texture);
            /*
            void loadImage2DArray(const std::string& name, const gli::texture& texture);

            void loadImage1D(const std::string& name, const gli::texture& texture);
            void loadImage1DArray(const std::string& name, const gli::texture& texture);

            void loadImage3D(const std::string& name, const gli::texture& texture);

            void loadImageCube(const std::string& name, const gli::texture& texture);
            */
        };

        /*struct Texture {
            ImageInfo imageInfo;
            vk::ImageLayout imageLayout;
            MemoryBlock memBlock;
            vk::ImageView view;
            uint32_t mipLevels;
            uint32_t layerCount;
            vk::DescriptorImageInfo descriptor;
            vk::Sampler sampler;
        };*/
    }
}

#endif // !_MX_VK_IMAGE_H_
