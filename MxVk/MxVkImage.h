#pragma once
#ifndef MX_VK_IMAGE_H_
#define MX_VK_IMAGE_H_

#include <gli/gli.hpp>
#include "../Utils/MxUtils.h"
#include "MxVkCore.h"
#include "MxVkAllocator.h"
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
            vk::Image CreateImage2D(Core& _core,
                                    const vk::Extent2D& _extent,
                                    const vk::Format _format,
                                    const vk::ImageUsageFlags& _usage,
                                    const uint32_t _mipLevels = 1, const uint32_t _arrayLayers = 1,
                                    const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
                                    const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
                                    const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
                                    const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive);

            vk::ImageView CreateImageView2D(const vk::Device& _device,
                                            const vk::Image& _image,
                                            const vk::Format _format,
                                            const vk::ImageAspectFlags& _aspectFlags,
                                            const uint32_t _baseMipLevel = 0,
                                            const uint32_t _levelCount = 1,
                                            const uint32_t _baseLayer = 0,
                                            const uint32_t _layerCount = 1);

            vk::DeviceMemory AllocateImageMemory(Core& _core,
                                                 const vk::Image& _image,
                                                 const vk::MemoryPropertyFlags& _properties);

            void TransferImageLayout(const vk::CommandBuffer& _cmdbuffer,
                                     vk::Image _image,
                                     vk::ImageLayout _oldImageLayout,
                                     vk::ImageLayout _newImageLayout,
                                     const vk::ImageSubresourceRange& _subresourceRange,
                                     vk::PipelineStageFlags _srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
                                     vk::PipelineStageFlags _dstStageMask = vk::PipelineStageFlagBits::eAllCommands);

            Image CreateDepthStencil(Core& _core,
                                     const vk::Extent2D& _extent,
                                     const vk::SampleCountFlagBits _sampleCount);
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

                TempData() : imageInfo(), ptr(nullptr) {
                }

                TempData(const std::string& _name, const ImageInfo& _imageInfo, const char* _p) :
                    name(_name), imageInfo(_imageInfo), ptr(_p) {
                }
            };
        public:
            ~ImageMgr() {
                destroy();
            }
            void init(std::shared_ptr<Core>& _core, std::shared_ptr<DeviceAllocator>& _allocator);

            void beginLoad(const vk::CommandBuffer& _cmd);

            void loadTexture(const std::string& _name, const gli::texture& _texture);
            // void loadTexture(const Utils::ImageData& _imageData);
            void endLoad();

            const ImageInfo& getImage(const std::string& name) {
                if (mImageInfos.count(name) == 0)
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

            vk::DeviceSize mBufferSize = 16 * 1024 * 1024;

            vk::Buffer mStagingBuffer;
            MemoryBlock mMemBlock;
            vk::MemoryRequirements mMemReq;
            Fence mFence;

            std::unordered_map<std::string, std::pair<ImageInfo, MemoryBlock>> mImageInfos;

            std::shared_ptr<DeviceAllocator> mpAllocator;

            void flush();

            static vk::DeviceSize LeastMultiple(vk::DeviceSize _a, vk::DeviceSize _b) {
                vk::DeviceSize m = _a, n = _b, c;
                while (_b != 0) {
                    c = _a % _b;
                    _a = _b;
                    _b = c;
                }
                return m * n / _a;
            }

            static vk::Format GliToVulkanFormat(gli::format _format);

            void loadImage2D(const std::string& _name, const gli::texture& _texture);
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

#endif // !MX_VK_IMAGE_H_
