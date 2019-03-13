#include "MxVkImage.h"


namespace Mix {
    namespace Graphics {
        namespace Tools {
            vk::Image createImage2D(const Core & core, const vk::Extent2D & extent, const vk::Format format, const vk::ImageUsageFlags & usage, const uint32_t mipLevels, const uint32_t arrayLayers, const vk::SampleCountFlagBits sampleCount, const vk::ImageLayout initialLayout, const vk::ImageTiling tiling, const vk::SharingMode sharingMode) {
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

                return core.device().createImage(createInfo);
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

            vk::DeviceMemory allocateImageMemory(const Core & core, const vk::Image & image, const vk::MemoryPropertyFlags & properties) {
                vk::MemoryRequirements memRq = core.device().getImageMemoryRequirements(image);

                vk::MemoryAllocateInfo allocInfo;
                allocInfo.allocationSize = memRq.size;
                allocInfo.memoryTypeIndex = core.getMemoryTypeIndex(memRq.memoryTypeBits, properties);

                return core.device().allocateMemory(allocInfo);
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

            Image createDepthStencil(const Core & core, const vk::Extent2D & extent, const vk::SampleCountFlagBits sampleCount) {
                static vk::Format candidates[] = {
                    vk::Format::eD32SfloatS8Uint,
                    vk::Format::eD24UnormS8Uint,
                    vk::Format::eD16UnormS8Uint
                };

                vk::Format format;
                vk::FormatProperties prop;
                for (auto candidate : candidates) {
                    if (core.checkFormatFeatureSupport(candidate,
                        vk::ImageTiling::eOptimal,
                        vk::FormatFeatureFlagBits::eDepthStencilAttachment))
                        format = candidate;
                }

                Image image;
                image.image = createImage2D(core,
                                            extent,
                                            format,
                                            vk::ImageUsageFlagBits::eDepthStencilAttachment);

                image.memory = allocateImageMemory(core, image.image, vk::MemoryPropertyFlagBits::eDeviceLocal);
                core.device().bindImageMemory(image.image, image.memory, 0);
                image.extent = vk::Extent3D(extent, 1);
                image.format = format;

                return image;
            }
        }
    }
}