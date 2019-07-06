#include "MxTexture.h"
#include "../../Object/MxObject.h"
#include "../../Vulkan/MxVkGraphics.h"
#include "../../Vulkan/Buffers/MxVkBufferTransfer.h"
#include "../../Vulkan/CommandBuffer/MxVkCommanddBufferHandle.h"
#include "../../../MixEngine.h"

namespace Mix {
	namespace Resource {
		Texture2D::Texture2D(const uint32_t _width,
							 const uint32_t _height,
							 vk::Format _format,
							 const vk::DeviceSize& _size,
							 const char* _data,
							 std::optional<vk::SamplerCreateInfo> _samplerInfo) {
			auto vulkan = MixEngine::Instance().getModule<Graphics::Vulkan>();

			mData->image = std::make_shared<Graphics::Image>(vulkan->getAllocator(),
															 vk::ImageType::e2D,
															 vk::MemoryPropertyFlagBits::eDeviceLocal,
															 vk::Extent3D(_width, _height, 1),
															 _format,
															 vk::ImageUsageFlagBits::eTransferDst |
															 vk::ImageUsageFlagBits::eSampled);

			mData->imageView = Graphics::Image::CreateVkImageView2DUnique(vulkan->getLogicalDevice()->get(),
																		  mData->image->get(),
																		  _format,
																		  vk::ImageAspectFlagBits::eColor);

			if (_samplerInfo) {
				mData->sampler = vulkan->getLogicalDevice()->get().createSamplerUnique(_samplerInfo.value());
			}
			else {
				vk::SamplerCreateInfo samplerInfo;
				samplerInfo.addressModeU = vk::SamplerAddressMode::eClampToBorder;
				samplerInfo.addressModeV = vk::SamplerAddressMode::eClampToBorder;
				samplerInfo.addressModeW = vk::SamplerAddressMode::eClampToBorder;
				samplerInfo.magFilter = vk::Filter::eLinear;
				samplerInfo.minFilter = vk::Filter::eLinear;
				samplerInfo.mipmapMode = vk::SamplerMipmapMode::eLinear;

				mData->sampler = vulkan->getLogicalDevice()->get().createSamplerUnique(samplerInfo);
			}


			// upload data (if has) and change the image layout to [ eShaderReadOnlyOptimal ]

			auto handle = std::make_shared<Graphics::CommandBufferHandle>(vulkan->getTransferCommandPool());

			vk::ImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			handle->begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);
			 // if has data to upload
			if (_data) {
				vk::BufferImageCopy bufferCopyRegion;
				bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
				bufferCopyRegion.imageSubresource.mipLevel = 0;
				bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
				bufferCopyRegion.imageSubresource.layerCount = 1;
				bufferCopyRegion.imageExtent = mData->image->extent();
				bufferCopyRegion.bufferOffset = 0;

				Graphics::Image::TransferVkImageLayout(handle->get(), mData->image->get(),
													   vk::ImageLayout::eUndefined,
													   vk::ImageLayout::eTransferDstOptimal,
													   subresourceRange);

				handle->addCommand(Graphics::DataToImage(vulkan->getAllocator(), _data, _size, mData->image, bufferCopyRegion));

				// change the layout of the image to [ eShaderReadOnlyOptimal ]
				Graphics::Image::TransferVkImageLayout(handle->get(), mData->image->get(),
													   vk::ImageLayout::eTransferDstOptimal,
													   vk::ImageLayout::eShaderReadOnlyOptimal,
													   subresourceRange);
			}
			else {
				Graphics::Image::TransferVkImageLayout(handle->get(), mData->image->get(),
													   vk::ImageLayout::eUndefined,
													   vk::ImageLayout::eShaderReadOnlyOptimal,
													   subresourceRange);
			}

			handle->end();
			handle->submit();
			handle->wait();

		}

		void Texture2D::uploadData(const char* const _data, const vk::DeviceSize& _size) {
			const auto vulkan = MixEngine::Instance().getModule<Graphics::Vulkan>();
			auto handle = std::make_shared<Graphics::CommandBufferHandle>(vulkan->getTransferCommandPool());

			vk::ImageSubresourceRange subresourceRange;
			subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
			subresourceRange.baseMipLevel = 0;
			subresourceRange.levelCount = 1;
			subresourceRange.baseArrayLayer = 0;
			subresourceRange.layerCount = 1;

			vk::BufferImageCopy bufferCopyRegion;
			bufferCopyRegion.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
			bufferCopyRegion.imageSubresource.mipLevel = 0;
			bufferCopyRegion.imageSubresource.baseArrayLayer = 0;
			bufferCopyRegion.imageSubresource.layerCount = 1;
			bufferCopyRegion.imageExtent = mData->image->extent();
			bufferCopyRegion.bufferOffset = 0;

			handle->begin(vk::CommandBufferUsageFlagBits::eOneTimeSubmit);

			Graphics::Image::TransferVkImageLayout(handle->get(), mData->image->get(),
												   vk::ImageLayout::eShaderReadOnlyOptimal,
												   vk::ImageLayout::eTransferDstOptimal,
												   subresourceRange);

			handle->addCommand(Graphics::DataToImage(vulkan->getAllocator(), _data, _size, mData->image, bufferCopyRegion));

			Graphics::Image::TransferVkImageLayout(handle->get(), mData->image->get(),
												   vk::ImageLayout::eTransferDstOptimal,
												   vk::ImageLayout::eShaderReadOnlyOptimal,
												   subresourceRange);

			handle->submit();
			handle->wait();
		}
	}
}
