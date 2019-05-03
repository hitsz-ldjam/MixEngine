#include "MxVkImage.h"


namespace Mix {
    namespace Graphics {
        namespace Tools {
            vk::Image createImage2D(Core & core, const vk::Extent2D & extent, const vk::Format format, const vk::ImageUsageFlags & usage, const uint32_t mipLevels, const uint32_t arrayLayers, const vk::SampleCountFlagBits sampleCount, const vk::ImageLayout initialLayout, const vk::ImageTiling tiling, const vk::SharingMode sharingMode) {
                vk::ImageCreateInfo createInfo;
                createInfo.imageType = vk::ImageType::e2D;
                createInfo.extent = vk::Extent3D(extent, 1);
                createInfo.mipLevels = mipLevels;
                createInfo.arrayLayers = arrayLayers;
                createInfo.format = format;
                createInfo.tiling = tiling;
                createInfo.initialLayout = initialLayout;
                createInfo.usage = usage;
                createInfo.sharingMode = sharingMode;
                createInfo.samples = sampleCount;

                return core.GetDevice().createImage(createInfo);
            }

            vk::ImageView createImageView2D(const vk::Device & device, const vk::Image & image, const vk::Format format, const vk::ImageAspectFlags & aspectFlags, const uint32_t baseMipLevel, const uint32_t levelCount, const uint32_t baseLayer, const uint32_t layerCount) {
                vk::ImageViewCreateInfo createInfo;
                createInfo.image = image;
                createInfo.format = format;
                createInfo.viewType = vk::ImageViewType::e2D;
                createInfo.subresourceRange.aspectMask = aspectFlags;
                createInfo.subresourceRange.baseArrayLayer = baseLayer;
                createInfo.subresourceRange.layerCount = layerCount;
                createInfo.subresourceRange.baseMipLevel = baseMipLevel;
                createInfo.subresourceRange.levelCount = levelCount;

                return device.createImageView(createInfo);
            }

            vk::DeviceMemory allocateImageMemory(Core & core, const vk::Image & image, const vk::MemoryPropertyFlags & properties) {
                vk::MemoryRequirements memRq = core.GetDevice().getImageMemoryRequirements(image);

                vk::MemoryAllocateInfo allocInfo;
                allocInfo.allocationSize = memRq.size;
                allocInfo.memoryTypeIndex = core.GetMemoryTypeIndex(memRq.memoryTypeBits, properties);

                return core.GetDevice().allocateMemory(allocInfo);
            }

            void transferImageLayout(const vk::CommandBuffer & cmdbuffer, vk::Image image, vk::ImageLayout oldImageLayout, vk::ImageLayout newImageLayout, const vk::ImageSubresourceRange & subresourceRange, vk::PipelineStageFlags srcStageMask, vk::PipelineStageFlags dstStageMask) {
                // Create an image barrier object
                vk::ImageMemoryBarrier imageMemoryBarrier;
                imageMemoryBarrier.oldLayout = oldImageLayout; //旧的布局
                imageMemoryBarrier.newLayout = newImageLayout; //新的布局
                imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //当需要转移所有权(属于哪个队列簇)的时候
                imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED; //填入队列簇的索引
                imageMemoryBarrier.image = image;
                imageMemoryBarrier.subresourceRange = subresourceRange;

                // Source layouts (old)
                // Source access mask controls actions that have to be finished on the old layout
                // before it will be transitioned to the new layout
                switch (oldImageLayout) {
                case vk::ImageLayout::eUndefined:
                    // Image layout is undefined (or does not matter)
                    // Only valid as initial layout
                    // No flags required, listed only for completeness
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlags();
                    break;

                case vk::ImageLayout::ePreinitialized:
                    // Image is preinitialized
                    // Only valid as initial layout for linear images, preserves memory contents
                    // Make sure host writes have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite;
                    break;

                case vk::ImageLayout::eColorAttachmentOptimal:
                    // Image is a color attachment
                    // Make sure any writes to the color buffer have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                    break;

                case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                    // Image is a depth/stencil attachment
                    // Make sure any writes to the depth/stencil buffer have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    break;

                case vk::ImageLayout::eTransferSrcOptimal:
                    // Image is a transfer source 
                    // Make sure any reads from the image have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferRead;
                    break;

                case vk::ImageLayout::eTransferDstOptimal:
                    // Image is a transfer destination
                    // Make sure any writes to the image have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eTransferWrite;
                    break;

                case vk::ImageLayout::eShaderReadOnlyOptimal:
                    // Image is read by a shader
                    // Make sure any shader reads from the image have been finished
                    imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eShaderRead;
                    break;
                default:
                    // Other source layouts aren't handled (yet)
                    break;
                }

                // Target layouts (new)
                // Destination access mask controls the dependency for the new image layout
                switch (newImageLayout) {
                case vk::ImageLayout::eTransferDstOptimal:
                    // Image will be used as a transfer destination
                    // Make sure any writes to the image have been finished
                    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferWrite;
                    break;

                case vk::ImageLayout::eTransferSrcOptimal:
                    // Image will be used as a transfer source
                    // Make sure any reads from the image have been finished
                    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eTransferRead;
                    break;

                case vk::ImageLayout::eColorAttachmentOptimal:
                    // Image will be used as a color attachment
                    // Make sure any writes to the color buffer have been finished
                    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eColorAttachmentWrite;
                    break;

                case vk::ImageLayout::eDepthStencilAttachmentOptimal:
                    // Image layout will be used as a depth/stencil attachment
                    // Make sure any writes to depth/stencil buffer have been finished
                    imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | vk::AccessFlagBits::eDepthStencilAttachmentWrite;
                    break;

                case vk::ImageLayout::eShaderReadOnlyOptimal:
                    // Image will be read in a shader (sampler, input attachment)
                    // Make sure any writes to the image have been finished
                    if (!imageMemoryBarrier.srcAccessMask) {
                        imageMemoryBarrier.srcAccessMask = vk::AccessFlagBits::eHostWrite | vk::AccessFlagBits::eTransferWrite;
                    }
                    imageMemoryBarrier.dstAccessMask = vk::AccessFlagBits::eShaderRead;
                    break;
                default:
                    // Other source layouts aren't handled (yet)
                    break;
                }

                // Put barrier inside setup command buffer
                cmdbuffer.pipelineBarrier(srcStageMask,
                                          dstStageMask,
                                          vk::DependencyFlags(),
                                          {},
                                          {},
                                          imageMemoryBarrier);
            }

            Image createDepthStencil(Core & core, const vk::Extent2D & extent, const vk::SampleCountFlagBits sampleCount) {
                static vk::Format candidates[] = {
                    vk::Format::eD32SfloatS8Uint,
                    vk::Format::eD24UnormS8Uint,
                    vk::Format::eD16UnormS8Uint
                };

                vk::Format format;
                vk::FormatProperties prop;
                for (auto candidate : candidates) {
                    if (core.CheckFormatFeatureSupport(candidate,
                                                       vk::ImageTiling::eOptimal,
                                                       vk::FormatFeatureFlagBits::eDepthStencilAttachment)) {

                        format = candidate;
                        break;
                    }
                }

                Image image;
                image.image = createImage2D(core,
                                            extent,
                                            format,
                                            vk::ImageUsageFlagBits::eDepthStencilAttachment);

                image.memory = allocateImageMemory(core, image.image, vk::MemoryPropertyFlagBits::eDeviceLocal);
                core.GetDevice().bindImageMemory(image.image, image.memory, 0);
                image.extent = vk::Extent3D(extent, 1);
                image.format = format;

                return image;
            }
        }

#pragma region MyRegion

        void ImageMgr::init(std::shared_ptr<Core> & core, std::shared_ptr<DeviceAllocator>& allocator) {
            GraphicsComponent::Init(core);
            mpAllocator = allocator;

            vk::BufferCreateInfo createInfo;
            createInfo.usage = vk::BufferUsageFlagBits::eTransferSrc;
            createInfo.size = mBufferSize;
            createInfo.sharingMode = vk::SharingMode::eExclusive;

            mStagingBuffer = mCore->GetDevice().createBuffer(createInfo);

            mMemReq = mCore->GetDevice().getBufferMemoryRequirements(mStagingBuffer);

            mMemBlock = mpAllocator->Allocate(mMemReq.size, mMemReq.alignment, mCore->GetMemoryTypeIndex(mMemReq.memoryTypeBits,
                                                                                                         vk::MemoryPropertyFlagBits::eHostVisible |
                                                                                                         vk::MemoryPropertyFlagBits::eHostCoherent));

            mCore->GetDevice().bindBufferMemory(mStagingBuffer, mMemBlock.memory, mMemBlock.offset);

            mFence = mCore->GetSyncObjMgr().createFence();
            mCore->GetDevice().resetFences(mFence.get());
        }

        void ImageMgr::beginLoad(const vk::CommandBuffer & cmd) {
            if (!mBegin) {
                mBegin = true;
                mCmd = cmd;
            }
        }

        vk::Format ImageMgr::gliToVulkanFormat(gli::format format) {
            vk::Format result;
            switch (format) {
            case gli::FORMAT_RGBA8_UNORM_PACK8:
            case gli::FORMAT_RGBA8_UNORM_PACK32:
                result = vk::Format::eR8G8B8A8Unorm;
                break;
            case gli::FORMAT_BGRA8_UNORM_PACK8:
                result = vk::Format::eB8G8R8A8Unorm;
                break;
            default:
                result = vk::Format::eUndefined;
                break;
            }
            return result;
        }

        void ImageMgr::loadImage2D(const std::string & name, const gli::texture& texture) {
            // check if current size exceeds limits
            if (mCurrSize && (mCurrSize + Utils::Align(texture.size(), mMemReq.alignment)) >= mMemReq.size)
                flush();

            ImageInfo info;
            info.type = vk::ImageType::e2D;
            info.format = gliToVulkanFormat(texture.format());
            info.extent = vk::Extent3D(texture.extent().x, texture.extent().y, 1);
            info.size = texture.size();
            info.mipLevels = 1;
            info.arrayLevels = 1;

            vk::ImageCreateInfo createInfo;
            createInfo.imageType = info.type;
            createInfo.extent = info.extent;
            createInfo.mipLevels = 1;
            createInfo.arrayLayers = 1;
            createInfo.format = info.format;
            createInfo.tiling = vk::ImageTiling::eOptimal;
            createInfo.initialLayout = vk::ImageLayout::eUndefined;
            createInfo.usage = vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled;
            createInfo.sharingMode = vk::SharingMode::eExclusive;
            createInfo.samples = vk::SampleCountFlagBits::e1;

            info.image = mCore->GetDevice().createImage(createInfo);
            mDatas.emplace_back(name, info, static_cast<const char*>(texture.data()));

            mCurrSize += Utils::Align(info.size, mMemReq.alignment);
        }

        void ImageMgr::loadTexture(const std::string & name, const gli::texture & texture) {
            // check if mCmd is begin
            if (!mCmdBegin) {
                mCmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eSimultaneousUse));
                mCmdBegin = true;
            }

            // todo implement this function to load texture of all types
            switch (texture.target()) {
            /*case gli::target::TARGET_1D:
                break;
            case gli::target::TARGET_1D_ARRAY:
                break;*/
            case gli::target::TARGET_2D:
                loadImage2D(name, texture);
                break;
            /*case gli::target::TARGET_2D_ARRAY:
                break;
            case gli::target::TARGET_3D:
                break;
            case gli::target::TARGET_CUBE:
                break;
            case gli::target::TARGET_CUBE_ARRAY:
                break;*/
            default:
                std::cerr << "Not supported Type of texture" << std::endl;
                break;
            }
        }

        void ImageMgr::endLoad() {
            if (mCurrSize)
                flush();
            mCmd = nullptr;
        }

        void ImageMgr::destroy() {
            if (!mCore)
                return;

            for (auto& imageInfo : mImageInfos) {
                mCore->GetDevice().destroyImage(imageInfo.second.first.image);
                mpAllocator->Deallocate(imageInfo.second.second);
            }
            mImageInfos.clear();
            mCore->GetDevice().destroyBuffer(mStagingBuffer);
            mpAllocator->Deallocate(mMemBlock);

            mCore = nullptr;
            mpAllocator = nullptr;
        }

        void ImageMgr::flush() {
            vk::DeviceSize offset = 0;

            // copy data to staging memory
            for (auto& data : mDatas) {
                memcpy(static_cast<char*>(mMemBlock.ptr) + offset,
                       data.ptr,
                       static_cast<size_t>(data.imageInfo.size));
                offset += Utils::Align(data.imageInfo.size, mMemReq.alignment);
            }

            // Allocate memory on video memory for images
            offset = 0;
            for (auto& data : mDatas) {
                auto& imageInfo = data.imageInfo;
                MemoryBlock block = mpAllocator->Allocate(imageInfo.image, vk::MemoryPropertyFlagBits::eDeviceLocal);

                vk::BufferImageCopy bufferCopyRegion;
                bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                bufferCopyRegion.imageSubresource.mipLevel = 0;
                bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageExtent = imageInfo.extent;
                bufferCopyRegion.bufferOffset = offset;

                offset += Utils::Align(imageInfo.size, mMemReq.alignment);

                vk::ImageSubresourceRange subresourceRange;
                subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = 1;
                subresourceRange.baseArrayLayer = 0;
                subresourceRange.layerCount = 1;

                Tools::transferImageLayout(mCmd,
                                           imageInfo.image,
                                           vk::ImageLayout::eUndefined,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           subresourceRange,
                                           vk::PipelineStageFlagBits::eTopOfPipe,
                                           vk::PipelineStageFlagBits::eTransfer);

                mCmd.copyBufferToImage(mStagingBuffer,
                                       imageInfo.image,
                                       vk::ImageLayout::eTransferDstOptimal,
                                       bufferCopyRegion);

                Tools::transferImageLayout(mCmd,
                                           imageInfo.image,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           vk::ImageLayout::eShaderReadOnlyOptimal,
                                           subresourceRange,
                                           vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eFragmentShader);

                mImageInfos.insert(std::make_pair(data.name, std::make_pair(imageInfo, block)));
            }

            mCmd.end();
            mCmdBegin = false;

            vk::SubmitInfo submitInfo;
            submitInfo.commandBufferCount = 1;
            submitInfo.pCommandBuffers = &mCmd;

            mCore->GetQueues().transfer.value().submit(submitInfo, mFence.get());
            mCore->GetDevice().waitForFences(mFence.get(), VK_TRUE, std::numeric_limits<uint64_t>::max());
            mCore->GetDevice().resetFences(mFence.get());

            // clear all stored info waiting for next flush()
            mCurrSize = 0;
            mDatas.clear();
            /*vk::DeviceSize totalSize = 0;
            std::vector<vk::DeviceSize> sizes;
            sizes.reserve(mDatas.size());
            for (auto& data : mDatas) {
                auto& imageInfo = std::get<1>(data);
                sizes.push_back(mCore->device().getImageMemoryRequirements(imageInfo.image).size);
                totalSize += sizes.back();
            }

            vk::MemoryAllocateInfo allocInfo;
            allocInfo.allocationSize = totalSize;
            allocInfo.memoryTypeIndex = mCore->getMemoryTypeIndex(mCore->device().getImageMemoryRequirements(std::get<1>(mDatas[0]).image).memoryTypeBits,
                                                                  vk::MemoryPropertyFlagBits::eDeviceLocal);

            vk::DeviceMemory texMemory = mCore->device().allocateMemory(allocInfo);
            offset = 0;
            for (uint32_t i = 0; i < mDatas.size(); ++i) {
                mCore->device().bindImageMemory(std::get<1>(mDatas[i]).image, texMemory, offset);
                offset += sizes[i];
            }

            offset = 0;
            for (uint32_t i = 0; i < mDatas.size(); ++i) {
                auto& imageInfo = std::get<1>(mDatas[i]);

                vk::BufferImageCopy bufferCopyRegion;
                bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
                bufferCopyRegion.imageSubresource.mipLevel = 0;
                bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
                bufferCopyRegion.imageSubresource.layerCount = 1;
                bufferCopyRegion.imageExtent = imageInfo.extent;
                bufferCopyRegion.bufferOffset = offset;

                offset += align(imageInfo.size, mMemReq.alignment);

                vk::ImageSubresourceRange subresourceRange;
                subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
                subresourceRange.baseMipLevel = 0;
                subresourceRange.levelCount = 1;
                subresourceRange.baseArrayLayer = 0;
                subresourceRange.layerCount = 1;

                Tools::transferImageLayout(mCmd,
                                           imageInfo.image,
                                           vk::ImageLayout::eUndefined,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           subresourceRange,
                                           vk::PipelineStageFlagBits::eTopOfPipe,
                                           vk::PipelineStageFlagBits::eTransfer);

                mCmd.copyBufferToImage(mStagingBuffer,
                                       imageInfo.image,
                                       vk::ImageLayout::eTransferDstOptimal,
                                       bufferCopyRegion);

                Tools::transferImageLayout(mCmd,
                                           imageInfo.image,
                                           vk::ImageLayout::eTransferDstOptimal,
                                           vk::ImageLayout::eShaderReadOnlyOptimal,
                                           subresourceRange,
                                           vk::PipelineStageFlagBits::eTransfer,
                                           vk::PipelineStageFlagBits::eFragmentShader);
            }

            for (auto& data : mDatas) {
                mImageInfos.insert(std::make_pair(std::get<0>(data), std::get<1>(data)));
            }*/
        }
#pragma endregion

    }
}