#pragma once
#ifndef MX_VK_IMAGE_H_
#define MX_VK_IMAGE_H_

#include <gli/gli.hpp>
#include "../Memory/MxVkAllocator.h"

namespace Mix {
	namespace Vulkan {
		class CommandBufferHandle;
		class Image : GeneralBase::NoCopyBase {
		public:
			Image(std::shared_ptr<DeviceAllocator> _allocator,
				  const vk::ImageType _type,
				  const vk::MemoryPropertyFlags& _memProperty,
				  const vk::Extent3D& _extent,
				  const vk::Format _format,
				  const vk::ImageUsageFlags& _usage,
				  const uint32_t _mipLevels = 1,
				  const uint32_t _arrayLayers = 1,
				  const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
				  const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
				  const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
				  const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive,
				  const vk::ImageCreateFlagBits& _flag = {});

			Image(std::shared_ptr<DeviceAllocator> _allocator, const vk::ImageCreateInfo& _createInfo, const vk::MemoryPropertyFlags& _memProperty);

			Image(Image&& _other) noexcept { swap(_other); }

			Image& operator=(Image&& _other) noexcept { swap(_other); return *this; }

			void swap(Image& _other) noexcept;

			// void uploadData(const void* _data, const vk::DeviceSize& _dstOffset, const vk::DeviceSize& _size) const;

			~Image();


			static std::shared_ptr<Image> CreateDepthStencil(const std::shared_ptr<DeviceAllocator>& _allocator,
															 const vk::Extent2D& _extent,
															 const vk::SampleCountFlagBits _sampleCount);

			static vk::Image CreateVkImage(const vk::Device& _device,
										   const vk::ImageType _type,
										   const vk::Extent3D& _extent,
										   const vk::Format _format,
										   const vk::ImageUsageFlags& _usage,
										   const uint32_t _mipLevels = 1,
										   const uint32_t _arrayLayers = 1,
										   const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
										   const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
										   const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
										   const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive,
										   const vk::ImageCreateFlagBits& _flag = {});

			static vk::UniqueImage CreateVkImageUnique(const vk::Device& _device,
													   const vk::ImageType _type,
													   const vk::Extent3D& _extent,
													   const vk::Format _format,
													   const vk::ImageUsageFlags& _usage,
													   const uint32_t _mipLevels = 1,
													   const uint32_t _arrayLayers = 1,
													   const vk::SampleCountFlagBits _sampleCount = vk::SampleCountFlagBits::e1,
													   const vk::ImageLayout _initialLayout = vk::ImageLayout::eUndefined,
													   const vk::ImageTiling _tiling = vk::ImageTiling::eOptimal,
													   const vk::SharingMode _sharingMode = vk::SharingMode::eExclusive,
													   const vk::ImageCreateFlagBits& _flag = {});

			static vk::ImageView CreateVkImageView2D(const vk::Device& _device,
													 const vk::Image& _image,
													 const vk::Format _format,
													 const vk::ImageAspectFlags& _aspectFlags,
													 const uint32_t _baseMipLevel = 0,
													 const uint32_t _levelCount = 1,
													 const uint32_t _baseLayer = 0,
													 const uint32_t _layerCount = 1,
													 const vk::ComponentMapping& _componentMapping = vk::ComponentMapping(),
													 const vk::ImageViewCreateFlags& _flags = vk::ImageViewCreateFlags());

			static vk::UniqueImageView CreateVkImageView2DUnique(const vk::Device& _device,
																 const vk::Image& _image,
																 const vk::Format _format,
																 const vk::ImageAspectFlags& _aspectFlags,
																 const uint32_t _baseMipLevel = 0,
																 const uint32_t _levelCount = 1,
																 const uint32_t _baseLayer = 0,
																 const uint32_t _layerCount = 1,
																 const vk::ComponentMapping& _componentMapping = vk::ComponentMapping(),
																 const vk::ImageViewCreateFlags& _flags = vk::ImageViewCreateFlags());

			static void TransferVkImageLayout(const vk::CommandBuffer& _cmdbuffer,
											  vk::Image _image,
											  vk::ImageLayout _oldImageLayout,
											  vk::ImageLayout _newImageLayout,
											  const vk::ImageSubresourceRange& _subresourceRange,
											  vk::PipelineStageFlags _srcStageMask = vk::PipelineStageFlagBits::eAllCommands,
											  vk::PipelineStageFlags _dstStageMask = vk::PipelineStageFlagBits::eAllCommands);

			static bool HasDepth(vk::Format _format);

			static bool HasStencil(vk::Format _format);

			vk::ImageType type() const { return mType; }

			const vk::Image& get() const { return mImage; }

			vk::Format format() const { return mFormat; }

			const vk::Extent3D& extent() const { return mExtent; }

			uint32_t mipLevels() const { return mMipLevels; }

			uint32_t arrayLevels() const { return mArrayLevels; }

			vk::ImageLayout layout() const { return mLayout; }

			vk::ImageTiling tiling() const { return mTiling; }

			uint64_t size() const { return mMemory.size; }

			std::shared_ptr<DeviceAllocator> getAllocator() const { return mAllocator; }

			char* rawPtr() { return reinterpret_cast<char*>(mMemory.ptr); }

		private:
			std::shared_ptr<DeviceAllocator> mAllocator;

			vk::ImageType mType;
			vk::Image mImage;

			vk::Format mFormat;
			vk::Extent3D mExtent;

			MemoryBlock mMemory;

			uint32_t mMipLevels;
			uint32_t mArrayLevels;
			vk::ImageLayout mLayout;
			vk::ImageTiling mTiling;
		};
	}
}

#endif // !MX_VK_IMAGE_H_
